#include "Scene/TextSceneWriter.h"

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
	void TextSceneWriter::Write(Core::Scene& scene, const std::string& sceneName,
		const uuids::uuid& sceneUUID, const std::filesystem::path& outputPath)
	{
		m_NodeUUIDs.clear();
		
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
		node.insert("parent", parent ? uuids::to_string(m_NodeUUIDs[parent]) : std::string{});
		
		//rttr::type type = rttr::type::get(*object);
		//node.insert("type", type.get_name().to_string());

		toml::table properties;
		//for (auto& property : type.get_properties())
		//{
			//rttr::variant value = property.get_value(*object);
			//toml::table tomlValue = VariantToToml(value);
			//properties.insert(property.get_name().to_string(), tomlValue);
		//}
		node.insert("properties", properties);
		
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

	//toml::table TextSceneWriter::VariantToToml(const rttr::variant& variant)
	//{
	//	toml::table result;

	//	rttr::type type = variant.get_type();
	//	result.insert("type", type.get_name().to_string());

	//	if (type == rttr::type::get<int>())
	//		result.insert("value", variant.get_value<int>());
	//	else if (variant.is_type<float>())
	//		result.insert("value", variant.get_value<float>());
	//	else if (variant.is_type<bool>())
	//		result.insert("value", variant.get_value<bool>());
	//	else if (variant.is_type<std::string>())
	//		result.insert("value", variant.get_value<std::string>());
	//	else if (variant.is_type<uuids::uuid>())
	//		result.insert("value", uuids::to_string(variant.get_value<uuids::uuid>()));
	//	else if (type.is_class())
	//	{
	//		toml::table nested;
	//		for (auto& prop : type.get_properties())
	//		{
	//			rttr::variant propVariant = prop.get_value(variant);
	//			nested.insert(prop.get_name().to_string(), VariantToToml(propVariant));
	//		}
	//		result.insert("value", nested);
	//	}
	//	else
	//	{
	//		Core::Log::Warning("TextSceneWriter: Unsupported type: " + type.get_name().to_string());
	//	}
	//	
	//	return result;
	//}

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
