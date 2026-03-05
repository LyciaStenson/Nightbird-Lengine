#pragma once

#include <uuid.h>

#include <filesystem>
#include <unordered_map>

namespace Nightbird::Core
{
	class Scene;
	class SceneObject;
	class Mesh;
	struct Transform;
}

namespace Nightbird::Editor
{
	class BinaryWriter;

	class BinarySceneWriter
	{
	public:
		void Write(Core::Scene& scene, const uuids::uuid& sceneUUID,
			const std::unordered_map<const Core::Mesh*, uuids::uuid>& meshUUIDs,
			const std::filesystem::path& outputPath);

	private:
		std::unordered_map<const Core::SceneObject*, uuids::uuid> m_NodeUUIDs;
		const std::unordered_map<const Core::Mesh*, uuids::uuid>* m_MeshUUIDs = nullptr;

		void AssignNodeUUIDs(Core::SceneObject* object);

		void WriteNode(Core::SceneObject* object, const uuids::uuid& parentUUID, BinaryWriter& writer);

		void WriteTransform(const Core::Transform& transform, BinaryWriter& writer);

		uuids::uuid GenerateUUID() const;
	};
}