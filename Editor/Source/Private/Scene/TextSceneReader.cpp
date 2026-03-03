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
#include "Core/SceneInstance.h"
#include "Core/Log.h"

#include <toml.hpp>

namespace Nightbird::Editor
{
	TextSceneReader::TextSceneReader(ImportManager& importManager)
		: m_ImportManager(importManager)
	{

	}

	std::unique_ptr<Core::Scene> TextSceneReader::Read(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			Core::Log::Error("TextSceneReader: File not found: " + path.string());
			return nullptr;
		}

		toml::parse_result result = toml::parse_file(path.string());
		if (!result)
		{
			Core::Log::Error("TextSceneReader: Failed to parse: " + path.string());
			return nullptr;
		}

		toml::table& document = result.table();

		std::string sceneName = document["scene"]["name"].value_or(std::string{});
		std::string sceneUUIDString = document["scene"]["uuid"].value_or(std::string{});
		auto sceneUUID = uuids::uuid::from_string(sceneUUIDString);
		if (!sceneUUID)
		{
			Core::Log::Error("TextSceneReader: Invalid scene UUID in: " + path.string());
			return nullptr;
		}

		auto scene = std::make_unique<Core::Scene>();

		std::unordered_map<uuids::uuid, Core::SpatialObject*> nodeMap;
		std::unordered_map<uuids::uuid, std::unique_ptr<Core::SpatialObject>> ownedNodesMap;
		std::unordered_map<uuids::uuid, uuids::uuid> parentMap;

		auto* nodesArray = document["nodes"].as_array();
		if (!nodesArray)
		{
			Core::Log::Warning("TextSceneReader: No nodes found in: " + path.string());
			return scene;
		}

		for (auto& nodeValue : *nodesArray)
		{
			auto* nodeTable = nodeValue.as_table();
			if (!nodeTable)
				continue;

			std::string uuidString = (*nodeTable)["uuid"].value_or(std::string{});
			std::string name = (*nodeTable)["name"].value_or(std::string{});
			std::string type = (*nodeTable)["type"].value_or(std::string{});
			std::string parentString = (*nodeTable)["parent"].value_or(std::string{});

			auto uuid = uuids::uuid::from_string(uuidString);
			if (!uuid)
			{
				Core::Log::Warning("TextSceneReader: Skipping node with invalid UUID");
				continue;
			}

			glm::vec3 position = glm::vec3(0.0f);
			glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
			glm::vec3 scale = glm::vec3(1.0f);

			if (auto* nodePosition = (*nodeTable)["position"].as_array())
			{
				position.x = nodePosition->get(0)->value_or(0.0f);
				position.y = nodePosition->get(1)->value_or(0.0f);
				position.z = nodePosition->get(2)->value_or(0.0f);
			}
			if (auto* nodeRotation = (*nodeTable)["rotation"].as_array())
			{
				rotation.x = nodeRotation->get(0)->value_or(0.0f);
				rotation.y = nodeRotation->get(1)->value_or(0.0f);
				rotation.z = nodeRotation->get(2)->value_or(0.0f);
				rotation.w = nodeRotation->get(3)->value_or(1.0f);
			}
			if (auto* nodeScale = (*nodeTable)["scale"].as_array())
			{
				scale.x = nodeScale->get(0)->value_or(1.0f);
				scale.y = nodeScale->get(1)->value_or(1.0f);
				scale.z = nodeScale->get(2)->value_or(1.0f);
			}

			std::unique_ptr<Core::SpatialObject> object;

			if (type == "scene_instance")
			{
				std::string sceneUUIDString = (*nodeTable)["scene"].value_or(std::string{});
				auto sceneUUID = uuids::uuid::from_string(sceneUUIDString);
				if (!sceneUUID)
				{
					Core::Log::Warning("TextSceneReader: Skipping SceneInstance with invalid UUID");
					continue;
				}

				const AssetInfo* assetInfo = m_ImportManager.GetAssetInfo(*sceneUUID);
				if (!assetInfo)
				{
					Core::Log::Warning("TextSceneReader: Asset not found for UUID: " + sceneUUIDString);
					continue;
				}

				auto sceneInstance = m_ImportManager.Import(assetInfo->sourcePath);
				if (!sceneInstance)
				{
					Core::Log::Warning("TextSceneReader: Failed to import asset: " + assetInfo->sourcePath.string());
					continue;
				}
				
				object = std::move(sceneInstance);
			}
			else if (type == "mesh_instance")
			{
				object = std::make_unique<Core::MeshInstance>(name, nullptr);
			}
			else if (type == "directional_light")
			{
				auto directionalLight = std::make_unique<Core::DirectionalLight>(name);
				if (auto* color = (*nodeTable)["color"].as_array())
				{
					directionalLight->color.r = color->get(0)->value_or(1.0f);
					directionalLight->color.g = color->get(1)->value_or(1.0f);
					directionalLight->color.b = color->get(2)->value_or(1.0f);
				}
				directionalLight->intensity = (*nodeTable)["intensity"].value_or(1.0f);
				object = std::move(directionalLight);
			}
			else if (type == "point_light")
			{
				auto pointLight = std::make_unique<Core::PointLight>(name);
				if (auto* color = (*nodeTable)["color"].as_array())
				{
					pointLight->color.r = color->get(0)->value_or(1.0f);
					pointLight->color.g = color->get(1)->value_or(1.0f);
					pointLight->color.b = color->get(2)->value_or(1.0f);
				}
				pointLight->intensity = (*nodeTable)["intensity"].value_or(1.0f);
				pointLight->radius = (*nodeTable)["radius"].value_or(1.0f);
				object = std::move(pointLight);
			}
			else if (type == "camera")
			{
				auto camera = std::make_unique<Core::Camera>(name);
				camera->fov = (*nodeTable)["fov"].value_or(70.0f);
				object = std::move(camera);
			}
			else
			{
				object = std::make_unique<Core::SpatialObject>(name);
			}

			object->transform.position = position;
			object->transform.rotation = rotation;
			object->transform.scale = scale;

			Core::SpatialObject* objectPtr = object.get();
			nodeMap[*uuid] = objectPtr;
			ownedNodesMap[*uuid] = std::move(object);

			if (!parentString.empty())
			{
				auto parentUUID = uuids::uuid::from_string(parentString);
				if (parentUUID)
					parentMap[*uuid] = *parentUUID;
			}
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
					scene->GetRoot()->AddChild(std::move(ownedNodesMap[*uuid]));
			}
			else
			{
				scene->GetRoot()->AddChild(std::move(ownedNodesMap[*uuid]));
			}
		}

		std::string activeCameraUUIDString = document["scene"]["active_camera"].value_or(std::string{});
		auto activeCameraUUID = uuids::uuid::from_string(activeCameraUUIDString);
		if (activeCameraUUID && nodeMap.count(*activeCameraUUID))
		{
			auto* camera = dynamic_cast<Core::Camera*>(nodeMap[*activeCameraUUID]);
			if (camera)
				scene->SetActiveCamera(camera);
		}

		Core::Log::Info("TextSceneReader: Loaded scene: " + sceneName);
		return scene;
	}
}