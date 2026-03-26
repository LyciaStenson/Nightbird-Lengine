#include "Scene/BinarySceneWriter.h"

#include "Core/Transform.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "Core/SpatialObject.h"
#include "Core/Camera.h"
#include "Core/MeshInstance.h"
#include "Core/DirectionalLight.h"
#include "Core/PointLight.h"
#include "Core/AudioSource.h"
#include "Core/Log.h"

#include "Cook/BinaryWriter.h"
#include "Scene/SceneObjectType.h"

namespace Nightbird::Editor
{
	void BinarySceneWriter::Write(Core::SceneObject* root, const uuids::uuid& sceneUUID,
		const std::unordered_map<const Core::Mesh*, uuids::uuid>& meshUUIDs,
		const std::filesystem::path& outputPath, Endianness endianness,
		Core::Camera* activeCamera)
	{
		m_NodeUUIDs.clear();
		m_MeshUUIDs = &meshUUIDs;

		AssignNodeUUIDs(root, true);

		std::filesystem::create_directories(outputPath.parent_path());
		BinaryWriter writer(outputPath, endianness);

		// Type
		writer.WriteUInt8('S');
		writer.WriteUInt8('C');
		writer.WriteUInt8('N');
		writer.WriteUInt8('E');

		// Version
		writer.WriteUInt32(1);
		
		// Scene name
		std::string name = root->GetName();
		writer.WriteUInt32(static_cast<uint32_t>(name.size()));
		writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(name.data()), name.size());

		// Scene UUID
		auto sceneUUIDBytes = sceneUUID.as_bytes();
		writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(sceneUUIDBytes.data()), 16);

		// Active camera UUID
		if (activeCamera && m_NodeUUIDs.count(activeCamera))
		{
			auto cameraUUIDBytes = m_NodeUUIDs[activeCamera].as_bytes();
			writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(cameraUUIDBytes.data()), 16);
		}
		else
		{
			auto nullBytes = uuids::uuid{}.as_bytes();
			writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(nullBytes.data()), 16);
		}
		
		// Node count
		writer.WriteUInt32(static_cast<uint32_t>(m_NodeUUIDs.size() - 1));

		for (const auto& child : root->GetChildren())
		{
			Core::Log::Info("BinarySceneWriter: Root name: " + std::string(root->GetName()));
			Core::Log::Info("BinarySceneWriter: Node count before -1: " + std::to_string(m_NodeUUIDs.size()));
			Core::Log::Info("BinarySceneWriter: Children count: " + std::to_string(root->GetChildren().size()));
			WriteNode(child.get(), uuids::uuid{}, writer);
		}

		Core::Log::Info("BinarySceneWriter: Written binary scene: " + outputPath.string());
	}

	void BinarySceneWriter::AssignNodeUUIDs(Core::SceneObject* object, bool isRoot)
	{
		if (!object)
			return;

		m_NodeUUIDs[object] = GenerateUUID();

		if (!isRoot && object->HasSourceScene())
			return;
		
		for (const auto& child : object->GetChildren())
			AssignNodeUUIDs(child.get());
	}

	void BinarySceneWriter::WriteNode(Core::SceneObject* object, const uuids::uuid& parentUUID, BinaryWriter& writer)
	{
		if (!object)
			return;

		// Node UUID
		auto nodeUUIDBytes = m_NodeUUIDs[object].as_bytes();
		writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(nodeUUIDBytes.data()), 16);

		// Parent UUID
		auto parentUUIDBytes = parentUUID.as_bytes();
		writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(parentUUIDBytes.data()), 16);

		// Name
		std::string name = object->GetName();
		writer.WriteUInt32(static_cast<uint32_t>(name.size()));
		writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(name.data()), name.size());
		
		if (object->HasSourceScene())
		{
			// Has Scene UUID (true)
			writer.WriteUInt8(1);
			// Scene UUID
			auto uuidBytes = object->GetSourceSceneUUID().value().as_bytes();
			writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(uuidBytes.data()), 16);
		}
		else
		{
			// Has Scene UUID (false)
			writer.WriteUInt8(0);
		}
		
		// Node type
		if (auto* meshInstance = object->Cast<Core::MeshInstance>())
		{
			writer.WriteUInt8(static_cast<uint8_t>(Core::SceneObjectType::MeshInstance));
			WriteTransform(meshInstance->transform, writer);
			if (meshInstance->GetMesh() && m_MeshUUIDs->count(meshInstance->GetMesh().get()))
			{
				auto uuidBytes = m_MeshUUIDs->at(meshInstance->GetMesh().get()).as_bytes();
				writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(uuidBytes.data()), 16);
			}
			else
			{
				auto nullBytes = uuids::uuid{}.as_bytes();
				writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(nullBytes.data()), 16);
			}
		}
		else if (auto* directionalLight = object->Cast<Core::DirectionalLight>())
		{
			writer.WriteUInt8(static_cast<uint8_t>(Core::SceneObjectType::DirectionalLight));
			WriteTransform(directionalLight->transform, writer);
			writer.WriteFloat(directionalLight->color.r);
			writer.WriteFloat(directionalLight->color.g);
			writer.WriteFloat(directionalLight->color.b);
			writer.WriteFloat(directionalLight->intensity);
		}
		else if (auto* pointLight = object->Cast<Core::PointLight>())
		{
			writer.WriteUInt8(static_cast<uint8_t>(Core::SceneObjectType::PointLight));
			WriteTransform(pointLight->transform, writer);
			writer.WriteFloat(pointLight->color.r);
			writer.WriteFloat(pointLight->color.g);
			writer.WriteFloat(pointLight->color.b);
			writer.WriteFloat(pointLight->intensity);
			writer.WriteFloat(pointLight->radius);
		}
		else if (auto* camera = object->Cast<Core::Camera>())
		{
			writer.WriteUInt8(static_cast<uint8_t>(Core::SceneObjectType::Camera));
			WriteTransform(camera->transform, writer);
			writer.WriteFloat(camera->fov);
		}
		else if (auto* audioSource = object->Cast<Core::AudioSource>())
		{
			writer.WriteUInt8(static_cast<uint8_t>(Core::SceneObjectType::AudioSource));
			auto audioUUIDBytes = audioSource->GetAudioUUID().as_bytes();
			writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(audioUUIDBytes.data()), 16);
			writer.WriteUInt8(audioSource->GetLoop() ? 1 : 0);
			writer.WriteUInt8(audioSource->GetPlayOnStart() ? 1 : 0);
			writer.WriteFloat(audioSource->GetVolume());
		}
		else if (auto* spatialObject = object->Cast<Core::SpatialObject>())
		{
			writer.WriteUInt8(static_cast<uint8_t>(Core::SceneObjectType::SpatialObject));
			WriteTransform(spatialObject->transform, writer);
		}
		else
		{
			writer.WriteUInt8(static_cast<uint8_t>(Core::SceneObjectType::SceneObject));
		}

		if (object->HasSourceScene())
			return; // Do not serialize children of scene instance
		
		for (const auto& child : object->GetChildren())
			WriteNode(child.get(), m_NodeUUIDs[object], writer);
	}

	void BinarySceneWriter::WriteTransform(const Core::Transform& transform, BinaryWriter& writer)
	{
		writer.WriteFloat(transform.position.x);
		writer.WriteFloat(transform.position.y);
		writer.WriteFloat(transform.position.z);
		writer.WriteFloat(transform.rotation.x);
		writer.WriteFloat(transform.rotation.y);
		writer.WriteFloat(transform.rotation.z);
		writer.WriteFloat(transform.rotation.w);
		writer.WriteFloat(transform.scale.x);
		writer.WriteFloat(transform.scale.y);
		writer.WriteFloat(transform.scale.z);
	}

	uuids::uuid BinarySceneWriter::GenerateUUID() const
	{
		std::random_device randomDevice;

		auto seedData = std::array<int, std::mt19937::state_size>{};
		std::generate(std::begin(seedData), std::end(seedData), std::ref(randomDevice));
		std::seed_seq seq(std::begin(seedData), std::end(seedData));
		std::mt19937 generator(seq);
		uuids::uuid_random_generator gen{generator};

		return gen();
	}
}
