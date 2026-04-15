#pragma once

#include "Cook/Endianness.h"

#include <uuid.h>

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace Nightbird::Core
{
	class SceneObject;
	class Camera;
}

namespace Nightbird::Editor
{
	class BinaryWriter;

	class BinarySceneWriter
	{
	public:
		void Write(Core::SceneObject* root, const uuids::uuid& sceneUUID,
			const std::filesystem::path& outputPath, Endianness endianness,
			Core::Camera* activeCamera = nullptr);

	private:
		//struct LeafProperty
		//{
			//uint32_t nameHash;
			//rttr::variant variant;
		//};

		std::unordered_map<const Core::SceneObject*, uuids::uuid> m_NodeUUIDs;

		void AssignNodeUUIDs(Core::SceneObject* object, bool isRoot = false);
		void WriteNode(Core::SceneObject* object, const uuids::uuid& parentUUID, BinaryWriter& writer);
		//void CollectLeaves(const rttr::instance& instance, const rttr::type& type, std::vector<LeafProperty>& leaves);
		//void CollectLeavesRecursive(const rttr::variant& variant, const rttr::type& type, std::vector<LeafProperty>& leaves);
		//void WriteLeaf(const LeafProperty& leaf, BinaryWriter& writer);

		uuids::uuid GenerateUUID() const;
	};
}
