#pragma once

#include <uuid.h>
#include <toml.hpp>
#include <rttr/variant.h>

#include <string>
#include <filesystem>
#include <unordered_map>

namespace Nightbird::Core
{
	class Scene;
	class SceneObject;
	class Mesh;
}

namespace Nightbird::Editor
{
	class TextSceneWriter
	{
	public:
		void Write(Core::Scene& scene, const std::string& sceneName,
			const uuids::uuid& sceneUUID, const std::filesystem::path& outputPath);
			//const std::unordered_map<const Core::Mesh*, uuids::uuid>& meshUUIDs);

	private:
		std::unordered_map<const Core::SceneObject*, uuids::uuid> m_NodeUUIDs;
		//const std::unordered_map<const Core::Mesh*, uuids::uuid>* m_MeshUUIDs = nullptr;

		void AssignNodeUUIDs(Core::SceneObject* object);

		void WriteNode(Core::SceneObject* object, Core::SceneObject* parent, toml::array& nodesArray);

		toml::table VariantToToml(const rttr::variant& variant);

		uuids::uuid GenerateUUID() const;
	};
}