#include "Core/BinarySceneReader.h"

#include "Core/AssetLoader.h"
#include "Core/BinaryReader.h"

#include "Core/Transform.h"
#include "Core/SceneObject.h"
#include "Core/SpatialObject.h"
#include "Core/MeshInstance.h"
#include "Core/DirectionalLight.h"
#include "Core/PointLight.h"
#include "Core/Camera.h"
#include "Core/AudioSource.h"
#include "Core/Log.h"

#include <vector>

namespace Nightbird::Core
{
	BinarySceneReader::BinarySceneReader(AssetLoader& assetLoader)
		: m_AssetLoader(assetLoader)
	{

	}

	SceneReadResult BinarySceneReader::Read(const std::string& cookedDir, const uuids::uuid& uuid)
	{
		SceneReadResult result;
		result.root = std::make_unique<SceneObject>();

		std::string path = cookedDir + "/" + uuids::to_string(uuid) + ".nbscene";

		BinaryReader reader(path);
		if (!reader.IsValid())
		{
			Core::Log::Error("BinarySceneReader: Failed to open: " + path);
			return result;
		}

		// Validate type
		uint8_t type[4] = {};
		reader.ReadRawBytes(type, 4);
		if (type[0] != 'S' || type[1] != 'C' || type[2] != 'N' || type[3] != 'E')
		{
			Core::Log::Error("BinarySceneReader: Invalid type signature in: " + path);
			return result;
		}

		// Check Version
		uint32_t version = reader.ReadUInt32();
		if (version != 1)
		{
			Core::Log::Error("BinarySceneReader: Unsupported version: " + std::to_string(version));
			return result;
		}

		// Scene name
		uint32_t nameLength = reader.ReadUInt32();
		std::string sceneName(nameLength, '\0');
		reader.ReadRawBytes(reinterpret_cast<uint8_t*>(sceneName.data()), nameLength);
		result.root->SetName(sceneName);

		// Scene UUID
		std::array<uint8_t, 16> sceneUUIDBytes;
		reader.ReadRawBytes(sceneUUIDBytes.data(), 16);
		result.uuid = uuids::uuid(sceneUUIDBytes);

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

			// Source scene UUID
			uint8_t hasSceneUUID = reader.ReadUInt8();
			uuids::uuid sourceSceneUUID;
			if (hasSceneUUID)
			{
				std::array<uint8_t, 16> sceneUUIDBytes;
				reader.ReadRawBytes(sceneUUIDBytes.data(), 16);
				sourceSceneUUID = uuids::uuid(sceneUUIDBytes);
			}

			// Type Name
			uint32_t typeNameLength = reader.ReadUInt16();
			std::string typeName(typeNameLength, '\0');
			reader.ReadRawBytes(reinterpret_cast<uint8_t*>(typeName.data()), typeNameLength);

			// Property count
			uint16_t propCount = reader.ReadUInt16();

			// Construct object via TypeInfo factory
			const TypeInfo* typeInfo = TypeInfo::Find(typeName);
			std::unique_ptr<Core::SceneObject> object;

			if (typeInfo && typeInfo->factory)
			{
				OwnedObject owned = typeInfo->factory();
				if (owned.IsValid())
					object.reset(static_cast<SceneObject*>(owned.Release()));
			}

			if (!object)
			{
				Log::Warning("BinarySceneReader: Unknown type " + typeName + ", defaulting to SceneObject");
				object = std::make_unique<SceneObject>();
				// Skip all properties
				for (uint16_t i = 0; i < propCount; ++i)
				{
					reader.ReadUInt32(); // Hash
					uint16_t size = reader.ReadUInt16();
					if (size > 0)
					{
						std::vector<uint8_t> discard(size);
						reader.ReadRawBytes(discard.data(), size);
					}
				}
			}
			else
			{
				ReadProperties(object.get(), typeInfo, propCount, reader);
			}

			if (hasSceneUUID)
				object->SetSourceSceneUUID(sourceSceneUUID);

			if (nodeUUID == activeCameraUUID)
				result.activeCamera = Cast<Camera>(object.get());

			SceneObject* objectPtr = object.get();
			nodeMap[nodeUUID] = objectPtr;
			ownedNodeMap[nodeUUID] = std::move(object);
			nodeUUIDs.push_back(nodeUUID);

			if (!parentUUID.is_nil())
				parentMap[nodeUUID] = parentUUID;
		}

		// Build scene hierarchy
		for (const auto& nodeUUID : nodeUUIDs)
		{
			if (!ownedNodeMap.count(nodeUUID))
				continue;

			auto parentIt = parentMap.find(nodeUUID);
			if (parentIt != parentMap.end() && nodeMap.count(parentIt->second))
				nodeMap[parentIt->second]->AddChild(std::move(ownedNodeMap[nodeUUID]));
			else
				result.root->AddChild(std::move(ownedNodeMap[nodeUUID]));
		}

		Core::Log::Info("BinarySceneReader: Loaded scene: " + path);
		return result;
	}

	void BinarySceneReader::ReadProperties(SceneObject* object, const TypeInfo* typeInfo, uint16_t propCount, BinaryReader& reader)
	{
		uint8_t* objectBase = reinterpret_cast<uint8_t*>(object);

		for (uint16_t i = 0; i < propCount; ++i)
		{
			uint32_t hash = reader.ReadUInt32();
			uint16_t size = reader.ReadUInt16();
			ReadIntoDesc(objectBase, typeInfo, hash, size, reader);
		}
	}

	void BinarySceneReader::ReadIntoDesc(uint8_t* objectBase, const TypeInfo* typeInfo, uint32_t incomingHash, uint16_t incomingSize, BinaryReader& reader)
	{
		for (const TypeInfo* t = typeInfo; t!= nullptr; t = t->parent)
		{
			for (uint16_t i = 0; i < t->propCount; ++i)
			{
				const PropDesc& desc = t->props[i];
				if (desc.nameHash != incomingHash)
					continue;

				if (incomingSize == 0)
				{
					// Size of 0 means nested type
					if (desc.nestedType)
					{
						uint8_t* nestedBase = objectBase + desc.offset;
						uint16_t nestedPropCount = desc.nestedType->propCount;

						for (uint16_t np = 0; np < nestedPropCount; ++np)
						{
							uint32_t nestedHash = reader.ReadUInt32();
							uint16_t nestedSize = reader.ReadUInt16();
							ReadIntoDesc(nestedBase, desc.nestedType, nestedHash, nestedSize, reader);
						}
					}
					else
					{
						Log::Warning("BinarySceneReader: Nested type marker with no nestedType for hash " + std::to_string(incomingHash));
					}
				}
				else
				{
					if (incomingSize == desc.size)
					{
						reader.ReadRawBytes(objectBase + desc.offset, incomingSize);
					}
					else
					{
						Log::Warning("BinarySceneReader: Size mismatch for property " + std::to_string(incomingHash) + ": Expected " + std::to_string(desc.size) + " but got " + std::to_string(incomingSize));
						std::vector<uint8_t> discard(incomingSize);
						reader.ReadRawBytes(discard.data(), incomingSize);
					}
				}

				return;
			}
		}

		// Mo matching PropDesc found
		if (incomingSize > 0)
		{
			Log::Warning("BinarySceneReader: No matching PropDesc found for property " + std::to_string(incomingHash));
			std::vector<uint8_t> discard(incomingSize);
			reader.ReadRawBytes(discard.data(), incomingSize);
		}
		else
		{
			Log::Warning("BinarySceneReader: Unknown nested type with hash " + std::to_string(incomingHash));
		}
	}
}
