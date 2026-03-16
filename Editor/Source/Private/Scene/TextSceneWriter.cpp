#include "Scene/TextSceneWriter.h"

#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "Core/SpatialObject.h"
#include "Core/MeshInstance.h"
#include "Core/DirectionalLight.h"
#include "Core/PointLight.h"
#include "Core/Camera.h"
#include "Core/Log.h"

namespace Nightbird::Editor
{
	void TextSceneWriter::Write(Core::Scene& scene, const std::string& sceneName,
		const uuids::uuid& sceneUUID, const std::filesystem::path& outputPath)
		//const std::unordered_map<const Core::Mesh*, uuids::uuid>& meshUUIDs)
	{
		m_NodeUUIDs.clear();
		//m_MeshUUIDs = &meshUUIDs;

		AssignNodeUUIDs(scene.GetRoot());

		toml::table document;

		toml::table sceneTable;
		sceneTable.insert("name", sceneName);
		sceneTable.insert("uuid", uuids::to_string(sceneUUID));
		Core::Camera* activeCamera = scene.GetActiveCamera();
		sceneTable.insert("active_camera", uuids::to_string(m_NodeUUIDs[activeCamera]));
		document.insert("scene", sceneTable);

		toml::array nodesArray;
		for (const auto& child : scene.GetRoot()->GetChildren())
			WriteNode(child.get(), nullptr, nodesArray);

		document.insert("nodes", nodesArray);

		std::filesystem::create_directories(outputPath.parent_path());
		std::ofstream file(outputPath);
		file << document;
		file.close();

		Core::Log::Info("Written .ntscene: " + outputPath.string());
	}

	void TextSceneWriter::AssignNodeUUIDs(Core::SceneObject* object)
	{
		if (!object)
			return;

		m_NodeUUIDs[object] = GenerateUUID();
		
		if (object->HasSourceScene())
			return;
		
		for (const auto& child : object->GetChildren())
			AssignNodeUUIDs(child.get());
	}

	void TextSceneWriter::WriteNode(Core::SceneObject* object, Core::SceneObject* parent, toml::array& nodesArray)
	{
		if (!object)
			return;

		toml::table node;

		node.insert("uuid", uuids::to_string(m_NodeUUIDs[object]));
		node.insert("name", object->GetName());

		if (parent)
			node.insert("parent", uuids::to_string(m_NodeUUIDs[parent]));
		else
			node.insert("parent", std::string{});

		if (auto* spatialObject = object->Cast<Core::SpatialObject>())
		{
			toml::array position;
			position.push_back(spatialObject->transform.position.x);
			position.push_back(spatialObject->transform.position.y);
			position.push_back(spatialObject->transform.position.z);
			node.insert("position", position);

			toml::array rotation;
			rotation.push_back(spatialObject->transform.rotation.x);
			rotation.push_back(spatialObject->transform.rotation.y);
			rotation.push_back(spatialObject->transform.rotation.z);
			rotation.push_back(spatialObject->transform.rotation.w);
			node.insert("rotation", rotation);

			toml::array scale;
			scale.push_back(spatialObject->transform.scale.x);
			scale.push_back(spatialObject->transform.scale.y);
			scale.push_back(spatialObject->transform.scale.z);
			node.insert("scale", scale);
		}
		
		if (auto* directionalLight = object->Cast<Core::DirectionalLight>())
		{
			node.insert("type", std::string("directional_light"));

			toml::array color;
			color.push_back(directionalLight->color.r);
			color.push_back(directionalLight->color.g);
			color.push_back(directionalLight->color.b);
			node.insert("color", color);
			node.insert("intensity", directionalLight->intensity);
		}
		else if (auto* pointLight = object->Cast<Core::PointLight>())
		{
			node.insert("type", std::string("point_light"));

			toml::array color;
			color.push_back(pointLight->color.r);
			color.push_back(pointLight->color.g);
			color.push_back(pointLight->color.b);
			node.insert("color", color);
			node.insert("intensity", pointLight->intensity);
			node.insert("radius", pointLight->radius);
		}
		else if (auto* camera = object->Cast<Core::Camera>())
		{
			node.insert("type", std::string("camera"));
			node.insert("fov", camera->fov);
		}
		else if (object->Cast<Core::SceneObject>())
		{
			node.insert("type", std::string("spatial_object"));
		}
		else
		{
			node.insert("type", std::string("scene_object"));
		}

		if (object->HasSourceScene())
		{
			node.insert("scene_uuid", uuids::to_string(object->GetSourceSceneUUID().value()));
			nodesArray.push_back(node);
			return; // Do not serialize children of scene instance
		}

		nodesArray.push_back(node);

		for (const auto& child : object->GetChildren())
			WriteNode(child.get(), object, nodesArray);
	}

	uuids::uuid TextSceneWriter::GenerateUUID() const
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
