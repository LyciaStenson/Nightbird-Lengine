#pragma once

#include "Core/IAssetLoader.h"

#include "Core/SceneReadResult.h"

#include "Core/BinarySceneReader.h"
#include "Core/ProjectLoader.h"
#include "Core/MeshLoader.h"
#include "Core/TextureLoader.h"
#include "Core/MaterialLoader.h"
#include "Core/AudioLoader.h"

#include <uuid.h>

#include <string>
#include <memory>
#include <unordered_map>

namespace Nightbird::Core
{
	class SceneObject;
	class Mesh;
	class AudioAsset;
	
	struct ProjectInfo;

	class AssetLoader : public IAssetLoader
	{
	public:
		explicit AssetLoader(const std::string& cookedDir);
		
		std::shared_ptr<Mesh> LoadMesh(const uuids::uuid& uuid) override;
		std::shared_ptr<AudioAsset> LoadAudio(const uuids::uuid& uuid) override;

		SceneReadResult LoadScene(const uuids::uuid& uuid);

		ProjectInfo LoadProject();

	private:
		std::string m_CookedDir;

		std::unique_ptr<BinarySceneReader> m_SceneReader;
		std::unique_ptr<ProjectLoader> m_ProjectLoader;
		std::unique_ptr<TextureLoader> m_TextureLoader;
		std::unique_ptr<MaterialLoader> m_MaterialLoader;
		std::unique_ptr<MeshLoader> m_MeshLoader;
		std::unique_ptr<AudioLoader> m_AudioLoader;
		
		void LoadNestedScenes(SceneObject* object);
	};
}
