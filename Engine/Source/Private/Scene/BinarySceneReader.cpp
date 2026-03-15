#include "Scene/BinarySceneReader.h"

#include "Load/AssetLoader.h"
#include "Load/BinaryReader.h"

#include "Scene/SceneObjectType.h"

#include "Core/Transform.h"
#include "Core/SceneObject.h"
#include "Core/SpatialObject.h"
#include "Core/MeshInstance.h"
#include "Core/DirectionalLight.h"
#include "Core/PointLight.h"
#include "Core/Camera.h"
#include "Core/Log.h"

namespace Nightbird::Load
{
	BinarySceneReader::BinarySceneReader(AssetLoader& assetLoader)
		: m_AssetLoader(assetLoader)
	{

	}
	
	std::unique_ptr<Core::Scene> BinarySceneReader::ReadScene(const std::string& cookedDir, const uuids::uuid& uuid)
	{
		auto nodesRead = ReadNodes(cookedDir, uuid);
		auto scene = std::make_unique<Core::Scene>();
		for (auto& child : nodesRead.rootChildren)
			scene->GetRoot()->AddChild(std::move(child));
		if (nodesRead.activeCamera)
			scene->SetActiveCamera(nodesRead.activeCamera);
		return scene;
	}

	BinarySceneReader::ReadNodesResult BinarySceneReader::ReadNodes(const std::string& cookedDir, const uuids::uuid& uuid)
	{
		ReadNodesResult readNodesResult;

		std::string path = cookedDir + "/" + uuids::to_string(uuid) + ".nbscene";

		BinaryReader reader(path);
		if (!reader.IsValid())
		{
			Core::Log::Error("BinarySceneReader: Failed to open: " + path);
			return readNodesResult;
		}

		// Validate type		
		uint8_t type[4] = {};
		reader.ReadRawBytes(type, 4);
		if (type[0] != 'S' || type[1] != 'C' || type[2] != 'N' || type[3] != 'E')
		{
			Core::Log::Error("BinarySceneReader: Invalid type signature in: " + path);
			return readNodesResult;
		}

		// Check Version
		uint32_t version = reader.ReadUInt32();
		if (version != 1)
		{
			Core::Log::Error("BinarySceneReader: Unsupported version: " + std::to_string(version));
			return readNodesResult;
		}

		// Scene name
		uint32_t nameLength = reader.ReadUInt32();
		std::string sceneName(nameLength, '\0');
		reader.ReadRawBytes(reinterpret_cast<uint8_t*>(sceneName.data()), nameLength);

		// Scene UUID
		std::array<uint8_t, 16> sceneUUIDBytes;
		reader.ReadRawBytes(sceneUUIDBytes.data(), 16);
		uuids::uuid sceneUUID(sceneUUIDBytes);

		// Active camera UUID
		std::array<uint8_t, 16> activeCameraUUIDBytes;
		reader.ReadRawBytes(activeCameraUUIDBytes.data(), 16);
		uuids::uuid activeCameraUUID(activeCameraUUIDBytes);

		// Node count
		uint32_t nodeCount = reader.ReadUInt32();

		std::unordered_map<uuids::uuid, Core::SceneObject*> nodeMap;
		std::unordered_map<uuids::uuid, std::unique_ptr<Core::SceneObject>> ownedNodeMap;
		std::unordered_map<uuids::uuid, uuids::uuid> parentMap;
		std::vector<uuids::uuid> nodeUUIDs;

		for (uint32_t i = 0; i < nodeCount; i++)
		{
			// Node UUID
			std::array<uint8_t, 16> nodeUUIDBytes;
			reader.ReadRawBytes(nodeUUIDBytes.data(), 16);
			uuids::uuid nodeUUID(nodeUUIDBytes);

			// Parent UUID
			std::array<uint8_t, 16> parentUUIDBytes;
			reader.ReadRawBytes(parentUUIDBytes.data(), 16);
			uuids::uuid parentUUID(parentUUIDBytes);

			// Name
			uint32_t nodeNameLength = reader.ReadUInt32();
			std::string nodeName(nodeNameLength, '\0');
			reader.ReadRawBytes(reinterpret_cast<uint8_t*>(nodeName.data()), nodeNameLength);

			uint8_t hasSceneUUID = reader.ReadUInt8();
			uuids::uuid sourceSceneUUID;
			if (hasSceneUUID)
			{
				std::array<uint8_t, 16> sceneUUIDBytes;
				reader.ReadRawBytes(sceneUUIDBytes.data(), 16);
				sourceSceneUUID = uuids::uuid(sceneUUIDBytes);
			}

			// Type
			Core::SceneObjectType type = static_cast<Core::SceneObjectType>(reader.ReadUInt8());

			std::unique_ptr<Core::SceneObject> object;

			switch (type)
			{
			case Core::SceneObjectType::SceneObject:
			{
				object = std::make_unique<Core::SceneObject>(nodeName);
				break;
			}
			case Core::SceneObjectType::SpatialObject:
			{
				auto spatialObject = std::make_unique<Core::SpatialObject>(nodeName);
				ReadTransform(spatialObject->transform, reader);
				object = std::move(spatialObject);
				break;
			}
			case Core::SceneObjectType::MeshInstance:
			{
				auto meshInstance = std::make_unique<Core::MeshInstance>(nodeName, nullptr);
				ReadTransform(meshInstance->transform, reader);

				std::array<uint8_t, 16> meshUUIDBytes;
				reader.ReadRawBytes(meshUUIDBytes.data(), 16);
				uuids::uuid meshUUID(meshUUIDBytes);

				if (!meshUUID.is_nil())
				{
					auto mesh = m_AssetLoader.LoadMesh(meshUUID);
					if (mesh)
						meshInstance->SetMesh(mesh);
				}

				object = std::move(meshInstance);
				break;
			}
			case Core::SceneObjectType::DirectionalLight:
			{
				auto directionalLight = std::make_unique<Core::DirectionalLight>(nodeName);
				ReadTransform(directionalLight->transform, reader);

				directionalLight->color.r = reader.ReadFloat();
				directionalLight->color.g = reader.ReadFloat();
				directionalLight->color.b = reader.ReadFloat();
				directionalLight->intensity = reader.ReadFloat();

				object = std::move(directionalLight);
				break;
			}
			case Core::SceneObjectType::PointLight:
			{
				auto pointLight = std::make_unique<Core::PointLight>(nodeName);
				ReadTransform(pointLight->transform, reader);

				pointLight->color.r = reader.ReadFloat();
				pointLight->color.g = reader.ReadFloat();
				pointLight->color.b = reader.ReadFloat();
				pointLight->intensity = reader.ReadFloat();
				pointLight->radius = reader.ReadFloat();

				object = std::move(pointLight);
				break;
			}
			case Core::SceneObjectType::Camera:
			{
				auto camera = std::make_unique<Core::Camera>(nodeName);
				ReadTransform(camera->transform, reader);

				camera->fov = reader.ReadFloat();
				if (nodeUUID == activeCameraUUID)
					readNodesResult.activeCamera = camera.get();

				object = std::move(camera);
				break;
			}
			default:
			{
				Core::Log::Warning("BinarySceneReader: Unknown object type: " + std::to_string(static_cast<int>(type)) + ", defaulting to SceneObject");
				object = std::make_unique<Core::SceneObject>(nodeName);
				break;
			}
			}

			if (hasSceneUUID)
			{
				object->SetSourceSceneUUID(sourceSceneUUID);
				auto instancedNodesResult = ReadNodes(cookedDir, sourceSceneUUID);
				for (auto& child : instancedNodesResult.rootChildren)
					object->AddChild(std::move(child));
			}

			Core::SceneObject* objectPtr = object.get();
			nodeMap[nodeUUID] = objectPtr;
			ownedNodeMap[nodeUUID] = std::move(object);
			nodeUUIDs.push_back(nodeUUID);

			if (!parentUUID.is_nil())
				parentMap[nodeUUID] = parentUUID;
		}

		for (const auto& nodeUUID : nodeUUIDs)
		{
			if (!ownedNodeMap.count(nodeUUID))
				continue;

			auto parentIt = parentMap.find(nodeUUID);
			if (parentIt != parentMap.end() && nodeMap.count(parentIt->second))
				nodeMap[parentIt->second]->AddChild(std::move(ownedNodeMap[nodeUUID]));
			else
				readNodesResult.rootChildren.push_back(std::move(ownedNodeMap[nodeUUID]));
		}
		
		Core::Log::Info("BinarySceneReader: Loaded scene: " + path);
		return readNodesResult;
	}
	
	void BinarySceneReader::ReadTransform(Core::Transform& transform, BinaryReader& reader)
	{
		transform.position.x = reader.ReadFloat();
		transform.position.y = reader.ReadFloat();
		transform.position.z = reader.ReadFloat();
		transform.rotation.x = reader.ReadFloat();
		transform.rotation.y = reader.ReadFloat();
		transform.rotation.z = reader.ReadFloat();
		transform.rotation.w = reader.ReadFloat();
		transform.scale.x = reader.ReadFloat();
		transform.scale.y = reader.ReadFloat();
		transform.scale.z = reader.ReadFloat();
	}
}
