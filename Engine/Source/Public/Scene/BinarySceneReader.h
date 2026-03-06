#pragma once

#include "Core/Scene.h"
#include "Core/SceneInstance.h"

#include <uuid.h>

#include <memory>
#include <filesystem>

namespace Nightbird::Core
{
	class SceneObject;
	class Camera;
	struct Transform;
}

namespace Nightbird::Load
{
	class AssetLoader;
	class BinaryReader;

	class BinarySceneReader
	{
	public:
		BinarySceneReader(AssetLoader& assetLoader);

		std::unique_ptr<Core::Scene> ReadScene(const std::filesystem::path& cookedDir, const uuids::uuid& uuid);
		std::unique_ptr<Core::SceneInstance> ReadSceneInstance(const std::filesystem::path& cookedDir, const uuids::uuid& uuid);

	private:
		AssetLoader& m_AssetLoader;

		struct ReadNodesResult
		{
			std::string sceneName;
			uuids::uuid sceneUUID;
			Core::Camera* activeCamera = nullptr;
			std::vector<std::unique_ptr<Core::SceneObject>> rootChildren;
		};

		ReadNodesResult ReadNodes(const std::filesystem::path& cookedDir, const uuids::uuid& uuid);

		void ReadTransform(Core::Transform& transform, BinaryReader& reader);
	};
}