#include "Scene/TextSceneReader.h"

#include "Import/ImportManager.h"
#include "Import/AssetInfo.h"

#include "Core/Scene.h"
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
	TextSceneReader::TextSceneReader(ImportManager& importManager)
		: m_ImportManager(importManager)
	{

	}

	SceneReadResult TextSceneReader::Read(const std::filesystem::path& path)
	{
		SceneReadResult sceneReadResult;

		if (!std::filesystem::exists(path))
		{
			Core::Log::Error("TextSceneReader: File not found: " + path.string());
			return sceneReadResult;
		}

		toml::parse_result tomlParseResult = toml::parse_file(path.string());
		if (!tomlParseResult)
		{
			Core::Log::Error("TextSceneReader: Failed to parse: " + path.string());
			return sceneReadResult;
		}

		toml::table& document = tomlParseResult.table();

		std::string sceneName = document["scene"]["name"].value_or(std::string{});
		std::string sceneUUIDString = document["scene"]["uuid"].value_or(std::string{});
		auto sceneUUID = uuids::uuid::from_string(sceneUUIDString);
		if (!sceneUUID)
		{
			Core::Log::Error("TextSceneReader: Invalid scene UUID in: " + path.string());
			return sceneReadResult;
		}

		sceneReadResult.scene = std::make_unique<Core::Scene>();
		sceneReadResult.uuid = *sceneUUID;

		toml::array* nodesArray = document["nodes"].as_array();
		if (!nodesArray)
		{
			Core::Log::Warning("TextSceneReader: No nodes found in: " + path.string());
			return sceneReadResult;
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
			rttr::type type = rttr::type::get_by_name(typeName);
			if (type.is_valid())
			{
				rttr::variant variant = type.create(); // Missing name
				if (variant.is_valid() && variant.can_convert<Core::SceneObject*>())
				{
					object.reset(variant.get_value<Core::SceneObject*>());
					object->SetName(name);
				}
			}

			if (!object)
			{
				Core::Log::Warning("TextSceneReader: Unknown type: " + typeName + ", defaulting to SceneObject");
				object = std::make_unique<Core::SceneObject>(); // Missing name
				object->SetName(name);
			}

			if (auto* propertiesTable = (*nodeTable)["properties"].as_table())
			{
				rttr::instance objectInstance = *object;
				DeserializeToml(*propertiesTable, objectInstance);
			}

			std::string sceneUUIDString = (*nodeTable)["scene_uuid"].value_or(std::string{});
			if (!sceneUUIDString.empty())
			{
				auto sceneUUID = uuids::uuid::from_string(sceneUUIDString);
				if (sceneUUID)
				{
					object->SetSourceSceneUUID(*sceneUUID);
					auto importedRoot = m_ImportManager.LoadScene(*sceneUUID);
					if (importedRoot)
					{
						for (auto& child : importedRoot->GetChildren())
							object->AddChild(std::move(child));
					}
					else
					{
						Core::Log::Warning("TextSceneReader: Assetinfo not found for scene UUID: " + sceneUUIDString);
					}
				}
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
					sceneReadResult.scene->GetRoot()->AddChild(std::move(ownedNodesMap[*uuid]));
			}
			else
			{
				sceneReadResult.scene->GetRoot()->AddChild(std::move(ownedNodesMap[*uuid]));
			}
		}

		std::string activeCameraUUIDString = document["scene"]["active_camera"].value_or(std::string{});
		auto activeCameraUUID = uuids::uuid::from_string(activeCameraUUIDString);
		if (activeCameraUUID && nodeMap.count(*activeCameraUUID))
		{
			auto* camera = Cast<Core::Camera>(nodeMap[*activeCameraUUID]);
			if (camera)
				sceneReadResult.scene->SetActiveCamera(camera);
		}

		Core::Log::Info("TextSceneReader: Loaded scene: " + sceneName);
		return sceneReadResult;
	}

	void TextSceneReader::DeserializeToml(const toml::table& table, rttr::instance instance)
	{
		rttr::type type = instance.get_derived_type();
		if (!type.is_class())
			return;
		
		for (auto& prop : type.get_properties())
		{
			if (!prop.is_valid() || prop.is_readonly())
				continue;

			std::string propName = prop.get_name().to_string();
			const auto* propNode = table.get(propName);
			if (!propNode)
				continue;

			const auto* propTable = propNode->as_table();
			if (!propTable)
				continue;

			rttr::type propType = prop.get_type().get_raw_type();
			rttr::variant variant;
			
			if (propType.is_class())
			{
				rttr::variant propVariant = prop.get_value(instance);
				if (!propVariant.is_valid())
					continue;

				auto* valueTable = (*propTable)["value"].as_table();
				if (!valueTable)
					continue;

				rttr::instance propInstance = propVariant;
				if (!propInstance.is_valid())
					continue;

				DeserializeToml(*valueTable, propInstance);
				variant = propVariant;
			}
			else
			{
				variant = TableToVariant(*propTable);
			}

			if (variant.is_valid())
			{
				bool success = prop.set_value(instance, variant);
				if (!success)
					Core::Log::Warning("TextSceneReader: Failed to set value for property " + propName);
			}
			else
			{
				Core::Log::Warning("TextSceneReader: Failed to deserialize property " + propName);
			}
		}
	}

	rttr::variant TextSceneReader::TableToVariant(const toml::table& table)
	{
		std::string typeName = table["type"].value_or(std::string{});

		if (typeName == "int")
			return table["value"].value_or(0);
		else if (typeName == "float")
			return table["value"].value_or(0.0f);
		else if (typeName == "bool")
			return table["value"].value_or(false);
		else if (typeName == "string")
			return table["value"].value_or(std::string{});
		else if (typeName == "uuid")
		{
			std::string uuidString = table["value"].value_or(std::string{});
			auto uuid = uuids::uuid::from_string(uuidString);
			return uuid ? rttr::variant(*uuid) : rttr::variant{};
		}

		Core::Log::Warning("TextSceneReader: TableToVariant: Unknown type: " + typeName);
		return {};
	}
}
