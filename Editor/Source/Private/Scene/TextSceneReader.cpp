#include "Scene/TextSceneReader.h"

#include "Import/AssetInfo.h"

#include "Core/SceneObject.h"
#include "Core/SpatialObject.h"
#include "Core/MeshInstance.h"
#include "Core/DirectionalLight.h"
#include "Core/PointLight.h"
#include "Core/Camera.h"
#include "Core/AudioSource.h"
#include "Core/Log.h"

namespace Nightbird::Editor
{
	Core::SceneReadResult TextSceneReader::Read(const std::filesystem::path& path)
	{
		Core::SceneReadResult result;

		if (!std::filesystem::exists(path))
		{
			Core::Log::Error("TextSceneReader: File not found: " + path.string());
			return result;
		}

		toml::parse_result tomlResult = toml::parse_file(path.string());
		if (!tomlResult)
		{
			Core::Log::Error("TextSceneReader: Failed to parse: " + path.string());
			return result;
		}

		toml::table& document = tomlResult.table();

		std::string sceneName = document["scene"]["name"].value_or(std::string{});
		std::string sceneUUIDString = document["scene"]["uuid"].value_or(std::string{});
		auto sceneUUID = uuids::uuid::from_string(sceneUUIDString);
		if (!sceneUUID)
		{
			Core::Log::Error("TextSceneReader: Invalid scene UUID in: " + path.string());
			return result;
		}

		result.root = std::make_unique<Core::SceneObject>();
		result.uuid = *sceneUUID;

		toml::array* nodesArray = document["nodes"].as_array();
		if (!nodesArray)
		{
			Core::Log::Warning("TextSceneReader: No nodes found in: " + path.string());
			return result;
		}

		std::unordered_map<uuids::uuid, Core::SceneObject*> nodeMap;
		std::unordered_map<uuids::uuid, std::unique_ptr<Core::SceneObject>> ownedNodesMap;

		for (auto& nodeValue : *nodesArray)
		{
			auto* nodeTable = nodeValue.as_table();
			if (!nodeTable)
				continue;

			std::string uuidString = (*nodeTable)["uuid"].value_or(std::string{});
			std::string name = (*nodeTable)["name"].value_or(std::string{});
			std::string typeName = (*nodeTable)["type"].value_or(std::string{});
			
			auto uuid = uuids::uuid::from_string(uuidString);
			if (!uuid)
			{
				Core::Log::Warning("TextSceneReader: Skipping node with invalid UUID");
				continue;
			}

			std::unique_ptr<Core::SceneObject> object;
			//rttr::type type = rttr::type::get_by_name(typeName);
			//if (type.is_valid())
			//{
			//	rttr::variant variant = type.create();
			//	if (variant.is_valid() && variant.can_convert<Core::SceneObject*>())
			//	{
			//		object.reset(variant.get_value<Core::SceneObject*>());
			//		object->SetName(name);
			//	}
			//}

			if (!object)
			{
				Core::Log::Warning("TextSceneReader: Unknown type: " + typeName + ", defaulting to SceneObject");
				object = std::make_unique<Core::SceneObject>();
				object->SetName(name);
			}

			//if (auto* propertiesTable = (*nodeTable)["properties"].as_table())
			//{
			//	rttr::instance objectInstance = *object;
			//	DeserializeToml(*propertiesTable, objectInstance);
			//}

			std::string sceneUUIDString = (*nodeTable)["scene_uuid"].value_or(std::string{});
			if (!sceneUUIDString.empty())
			{
				auto sceneUUID = uuids::uuid::from_string(sceneUUIDString);
				if (sceneUUID)
					object->SetSourceSceneUUID(*sceneUUID);
			}

			Core::SceneObject* objectPtr = object.get();
			nodeMap[*uuid] = objectPtr;
			ownedNodesMap[*uuid] = std::move(object);
		}

		for (auto& nodeValue : *nodesArray)
		{
			auto* nodeTable = nodeValue.as_table();
			if (!nodeTable)
				continue;

			std::string uuidString = (*nodeTable)["uuid"].value_or(std::string{});
			std::string parentString = (*nodeTable)["parent"].value_or(std::string{});

			auto uuid = uuids::uuid::from_string(uuidString);
			if (!uuid || !ownedNodesMap.count(*uuid))
				continue;

			if (!parentString.empty())
			{
				auto parentUUID = uuids::uuid::from_string(parentString);
				if (parentUUID && nodeMap.count(*parentUUID))
					nodeMap[*parentUUID]->AddChild(std::move(ownedNodesMap[*uuid]));
				else
					result.root->AddChild(std::move(ownedNodesMap[*uuid]));
			}
			else
			{
				result.root->AddChild(std::move(ownedNodesMap[*uuid]));
			}
		}

		std::string activeCameraUUIDString = document["scene"]["active_camera"].value_or(std::string{});
		auto activeCameraUUID = uuids::uuid::from_string(activeCameraUUIDString);
		if (activeCameraUUID && nodeMap.count(*activeCameraUUID))
		{
			result.activeCamera = Cast<Core::Camera>(nodeMap[*activeCameraUUID]);
		}

		Core::Log::Info("TextSceneReader: Loaded scene: " + sceneName);
		return result;
	}

	//void TextSceneReader::DeserializeToml(const toml::table& table, rttr::instance instance)
	//{
	//	rttr::type type = instance.get_derived_type();
	//	if (!type.is_class())
	//		return;
	//	
	//	for (auto& prop : type.get_properties())
	//	{
	//		if (!prop.is_valid() || prop.is_readonly())
	//			continue;

	//		std::string propName = prop.get_name().to_string();
	//		const auto* propNode = table.get(propName);
	//		if (!propNode)
	//			continue;

	//		const auto* propTable = propNode->as_table();
	//		if (!propTable)
	//			continue;

	//		rttr::type propType = prop.get_type().get_raw_type();
	//		rttr::variant variant;
	//		
	//		if (propType.is_class())
	//		{
	//			rttr::variant propVariant = prop.get_value(instance);
	//			if (!propVariant.is_valid())
	//				continue;

	//			auto* valueTable = (*propTable)["value"].as_table();
	//			if (!valueTable)
	//				continue;

	//			rttr::instance propInstance = propVariant;
	//			if (!propInstance.is_valid())
	//				continue;

	//			DeserializeToml(*valueTable, propInstance);
	//			variant = propVariant;
	//		}
	//		else
	//		{
	//			variant = TableToVariant(*propTable);
	//		}

	//		if (variant.is_valid())
	//		{
	//			bool success = prop.set_value(instance, variant);
	//			if (!success)
	//				Core::Log::Warning("TextSceneReader: Failed to set value for property " + propName);
	//		}
	//		else
	//		{
	//			Core::Log::Warning("TextSceneReader: Failed to deserialize property " + propName);
	//		}
	//	}
	//}

	//rttr::variant TextSceneReader::TableToVariant(const toml::table& table)
	//{
	//	std::string typeName = table["type"].value_or(std::string{});

	//	if (typeName == "int")
	//		return table["value"].value_or(0);
	//	else if (typeName == "float")
	//		return table["value"].value_or(0.0f);
	//	else if (typeName == "bool")
	//		return table["value"].value_or(false);
	//	else if (typeName == "string")
	//		return table["value"].value_or(std::string{});
	//	else if (typeName == "uuid")
	//	{
	//		std::string uuidString = table["value"].value_or(std::string{});
	//		auto uuid = uuids::uuid::from_string(uuidString);
	//		return uuid ? rttr::variant(*uuid) : rttr::variant{};
	//	}

	//	Core::Log::Warning("TextSceneReader: TableToVariant: Unknown type: " + typeName);
	//	return {};
	//}
}
