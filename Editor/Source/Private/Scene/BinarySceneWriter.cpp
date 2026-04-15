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
			auto bytes = m_NodeUUIDs[activeCamera].as_bytes();
			writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(bytes.data()), 16);
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
		//rttr::type type = rttr::type::get(*object);
		//std::string typeName = type.get_name().to_string();
		//writer.WriteUInt16(static_cast<uint16_t>(typeName.size()));
		//writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(typeName.data()), typeName.size());

		// Collect leaf properties
		//std::vector<LeafProperty> leaves;
		//CollectLeaves(rttr::instance(*object), type, leaves);

		// Leaves size
		//writer.WriteUInt16(static_cast<uint16_t>(leaves.size()));

		//for (const auto& leaf : leaves)
			//WriteLeaf(leaf, writer);

		// Do not serialize children of scene instance
		if (object->HasSourceScene())
			return;

		for (const auto& child : object->GetChildren())
			WriteNode(child.get(), m_NodeUUIDs[object], writer);
	}

	//void BinarySceneWriter::CollectLeaves(const rttr::instance& instance, const rttr::type& type, std::vector<LeafProperty>& leaves)
	//{
	//	for (auto& prop : type.get_properties())
	//	{
	//		rttr::variant variant = prop.get_value(instance);
	//		rttr::type propType = variant.get_type();
	//		uint32_t nameHash = Nightbird::FNVHash(prop.get_name().to_string());

	//		if (propType == rttr::type::get<int>() ||
	//			propType == rttr::type::get<float>() ||
	//			propType == rttr::type::get<bool>() ||
	//			propType == rttr::type::get<std::string>() ||
	//			propType == rttr::type::get<uuids::uuid>())
	//		{
	//			leaves.push_back({ nameHash, variant });
	//		}
	//		else if (propType.is_class() && !propType.get_properties().empty())
	//		{
	//			leaves.push_back({ nameHash, variant });
	//			CollectLeavesRecursive(variant, propType, leaves);
	//		}
	//		else
	//		{
	//			Core::Log::Warning("BinarySceneWriter: Skipping unsupported property " + prop.get_name().to_string() + " of type " + propType.get_name().to_string());
	//		}
	//	}
	//}

	//void BinarySceneWriter::CollectLeavesRecursive(const rttr::variant& variant, const rttr::type& type, std::vector<LeafProperty>& leaves)
	//{
	//	for (auto& prop : type.get_properties())
	//	{
	//		rttr::variant propVariant = prop.get_value(variant);
	//		rttr::type propType = propVariant.get_type();
	//		uint32_t nameHash = Nightbird::FNVHash(prop.get_name().to_string());

	//		if (propType == rttr::type::get<int>() ||
	//			propType == rttr::type::get<float>() ||
	//			propType == rttr::type::get<bool>() ||
	//			propType == rttr::type::get<std::string>() ||
	//			propType == rttr::type::get<uuids::uuid>())
	//		{
	//			leaves.push_back({ nameHash, propVariant});
	//		}
	//		else if (propType.is_class() && !propType.get_properties().empty())
	//		{
	//			leaves.push_back({ nameHash, propVariant});
	//			CollectLeavesRecursive(propVariant, propType, leaves);
	//		}
	//		else
	//		{
	//			Core::Log::Warning("BinarySceneWriter: Skipping unsupported property " + prop.get_name().to_string() + " of type " + propType.get_name().to_string());
	//		}
	//	}
	//}

	//void BinarySceneWriter::WriteLeaf(const LeafProperty& leaf, BinaryWriter& writer)
	//{
	//	rttr::type type = leaf.variant.get_type();

	//	writer.WriteUInt32(leaf.nameHash);

	//	if (type == rttr::type::get<int>())
	//	{
	//		writer.WriteUInt16(sizeof(int32_t));
	//		writer.WriteInt32(leaf.variant.get_value<int>());
	//	}
	//	else if (type == rttr::type::get<float>())
	//	{
	//		writer.WriteUInt16(sizeof(float));
	//		writer.WriteFloat(leaf.variant.get_value<float>());
	//	}
	//	else if (type == rttr::type::get<bool>())
	//	{
	//		writer.WriteUInt16(sizeof(uint8_t));
	//		writer.WriteUInt8(leaf.variant.get_value<bool>() ? 1 : 0);
	//	}
	//	else if (type == rttr::type::get<std::string>())
	//	{
	//		const auto& value = leaf.variant.get_value<std::string>();
	//		writer.WriteUInt16(static_cast<uint16_t>(sizeof(uint32_t) + value.size()));
	//		writer.WriteUInt32(static_cast<uint32_t>(value.size()));
	//		writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(value.data()), value.size());
	//	}
	//	else if (type == rttr::type::get<uuids::uuid>())
	//	{
	//		writer.WriteUInt16(16);
	//		auto bytes = leaf.variant.get_value<uuids::uuid>().as_bytes();
	//		writer.WriteRawBytes(reinterpret_cast<const uint8_t*>(bytes.data()), 16);
	//	}
	//	else if (type.is_class())
	//	{
	//		// Struct
	//		writer.WriteUInt16(0);
	//	}
	//}

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
