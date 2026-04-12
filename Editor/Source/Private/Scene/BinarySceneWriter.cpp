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

#include "Core/SceneObjectType.h"

#include "Cook/BinaryWriter.h"

namespace Nightbird::Editor
{
	void BinarySceneWriter::Write(Core::SceneObject* root, const uuids::uuid& sceneUUID,
		const std::filesystem::path& outputPath, Endianness endianness,
		Core::Camera* activeCamera)
	{
		m_NodeUUIDs.clear();

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

		// Node count (minus root)
		writer.WriteUInt32(static_cast<uint32_t>(m_NodeUUIDs.size() - 1));

		for (const auto& child : root->GetChildren())
			WriteNode(child.get(), uuids::uuid{}, writer);

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
		//std::string name = object->GetName();
		//writer.WriteUInt32(static_cast<uint32_t>(name.size()));
		//writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(name.data()), name.size());

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

		// Type name
		rttr::type type = rttr::type::get(*object);
		std::string typeName = type.get_name().to_string();
		writer.WriteUInt16(static_cast<uint16_t>(typeName.size()));
		writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(typeName.data()), typeName.size());

		// Properties
		rttr::instance instance(*object);
		auto properties = type.get_properties();

		writer.WriteUInt16(static_cast<uint16_t>(properties.size()));

		for (auto& prop : properties)
		{
			uint32_t nameHash = Nightbird::FNVHash(prop.get_name().to_string());
		}

		// Node type
		if (auto* meshInstance = Cast<Core::MeshInstance>(object))
		{
			writer.WriteUInt8(static_cast<uint8_t>(Core::SceneObjectType::MeshInstance));
			//WriteTransform(meshInstance->m_Transform, writer);
			//if (meshInstance->GetMesh() && m_MeshUUIDs->count(meshInstance->GetMesh().get()))
			//{
				//auto uuidBytes = m_MeshUUIDs->at(meshInstance->GetMesh().get()).as_bytes();
				//writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(uuidBytes.data()), 16);
			//}
			//else
			//{
				//auto nullBytes = uuids::uuid{}.as_bytes();
				//writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(nullBytes.data()), 16);
			//}
		}
		else if (auto* directionalLight = Cast<Core::DirectionalLight>(object))
		{
			writer.WriteUInt8(static_cast<uint8_t>(Core::SceneObjectType::DirectionalLight));
			//WriteTransform(directionalLight->m_Transform, writer);
			writer.WriteFloat(directionalLight->m_Color.r);
			writer.WriteFloat(directionalLight->m_Color.g);
			writer.WriteFloat(directionalLight->m_Color.b);
			writer.WriteFloat(directionalLight->m_Intensity);
		}
		else if (auto* pointLight = Cast<Core::PointLight>(object))
		{
			writer.WriteUInt8(static_cast<uint8_t>(Core::SceneObjectType::PointLight));
			//WriteTransform(pointLight->m_Transform, writer);
			writer.WriteFloat(pointLight->m_Color.r);
			writer.WriteFloat(pointLight->m_Color.g);
			writer.WriteFloat(pointLight->m_Color.b);
			writer.WriteFloat(pointLight->m_Intensity);
			writer.WriteFloat(pointLight->m_Radius);
		}
		else if (auto* camera = Cast<Core::Camera>(object))
		{
			writer.WriteUInt8(static_cast<uint8_t>(Core::SceneObjectType::Camera));
			//WriteTransform(camera->m_Transform, writer);
			writer.WriteFloat(camera->m_Fov);
		}
		else if (auto* audioSource = Cast<Core::AudioSource>(object))
		{
			writer.WriteUInt8(static_cast<uint8_t>(Core::SceneObjectType::AudioSource));
			auto audioUUIDBytes = audioSource->GetAudioUUID().as_bytes();
			writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(audioUUIDBytes.data()), 16);
			writer.WriteUInt8(audioSource->GetLoop() ? 1 : 0);
			writer.WriteUInt8(audioSource->GetPlayOnStart() ? 1 : 0);
			writer.WriteFloat(audioSource->GetVolume());
		}
		else if (auto* spatialObject = Cast<Core::SpatialObject>(object))
		{
			writer.WriteUInt8(static_cast<uint8_t>(Core::SceneObjectType::SpatialObject));
			//WriteTransform(spatialObject->m_Transform, writer);
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

	void BinarySceneWriter::WriteProperties(const rttr::instance& instance, BinaryWriter& writer)
	{

	}

	void BinarySceneWriter::WriteVariant(const rttr::variant& variant, BinaryWriter& writer)
	{

	}

	// void BinarySceneWriter::WriteTransform(const Core::Transform& transform, BinaryWriter& writer)
	// {
	//	writer.WriteFloat(transform.position.x);
	//	writer.WriteFloat(transform.position.y);
	//	writer.WriteFloat(transform.position.z);
	//	writer.WriteFloat(transform.rotation.x);
	//	writer.WriteFloat(transform.rotation.y);
	//	writer.WriteFloat(transform.rotation.z);
	//	writer.WriteFloat(transform.rotation.w);
	//	writer.WriteFloat(transform.scale.x);
	//	writer.WriteFloat(transform.scale.y);
	//	writer.WriteFloat(transform.scale.z);
	// }

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
