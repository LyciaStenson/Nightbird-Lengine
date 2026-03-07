#pragma once

#include "Cook/Endianness.h"

#include <uuid.h>

#include <filesystem>
#include <unordered_map>

namespace Nightbird::Core
{
	class SceneObject;
	class Camera;
	class Mesh;
	struct Transform;
}

namespace Nightbird::Editor
{
	class BinaryWriter;

	class BinarySceneWriter
	{
	public:
		void Write(Core::SceneObject* root, const uuids::uuid& sceneUUID,
			const std::unordered_map<const Core::Mesh*, uuids::uuid>& meshUUIDs,
			const std::filesystem::path& outputPath, Endianness endianness,
			Core::Camera* activeCamera = nullptr);

	private:
		std::unordered_map<const Core::SceneObject*, uuids::uuid> m_NodeUUIDs;
		const std::unordered_map<const Core::Mesh*, uuids::uuid>* m_MeshUUIDs = nullptr;

		void AssignNodeUUIDs(Core::SceneObject* object, bool isRoot = false);

		void WriteNode(Core::SceneObject* object, const uuids::uuid& parentUUID, BinaryWriter& writer);

		void WriteTransform(const Core::Transform& transform, BinaryWriter& writer);

		uuids::uuid GenerateUUID() const;
	};
}