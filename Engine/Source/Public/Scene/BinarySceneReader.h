#pragma once

#include "Core/Scene.h"

#include <uuid.h>

#include <memory>
#include <string>

namespace Nightbird::Core
{
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
		
		std::unique_ptr<Core::Scene> ReadScene(const std::string& cookedDir, const uuids::uuid& uuid);

	private:
		AssetLoader& m_AssetLoader;

		struct ReadNodesResult
		{
			std::string sceneName;
			uuids::uuid sceneUUID;
			Core::Camera* activeCamera = nullptr;
			std::vector<std::unique_ptr<Core::SceneObject>> rootChildren;
		};

		ReadNodesResult ReadNodes(const std::string& cookedDir, const uuids::uuid& uuid);

		void ReadTransform(Core::Transform& transform, BinaryReader& reader);
	};
}
