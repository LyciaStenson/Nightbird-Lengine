#pragma once

#include <uuid.h>

#include <string>
#include <memory>
#include <unordered_map>

namespace Nightbird::Core
{
	class Scene;
	class Mesh;
	class AudioAsset;

	class BinarySceneReader;
	class ProjectLoader;
	struct ProjectInfo;
	class TextureLoader;
	class MaterialLoader;
	class MeshLoader;
	class AudioLoader;

	class AssetLoader
	{
	public:
		AssetLoader(const std::string& cookedDir);

		ProjectInfo LoadProject();

		std::unique_ptr<Core::Scene> LoadScene(const uuids::uuid& uuid);

		std::shared_ptr<Core::Mesh> LoadMesh(const uuids::uuid& uuid);
		std::shared_ptr<Core::AudioAsset> LoadAudio(const uuids::uuid& uuid);

	private:
		std::string m_CookedDir;

		std::unique_ptr<BinarySceneReader> m_SceneReader;
		std::unique_ptr<ProjectLoader> m_ProjectLoader;
		std::unique_ptr<TextureLoader> m_TextureLoader;
		std::unique_ptr<MaterialLoader> m_MaterialLoader;
		std::unique_ptr<MeshLoader> m_MeshLoader;
		std::unique_ptr<AudioLoader> m_AudioLoader;

		std::unordered_map<uuids::uuid, std::shared_ptr<Core::Mesh>> m_MeshCache;
		std::unordered_map<uuids::uuid, std::shared_ptr<Core::AudioAsset>> m_AudioCache;
	};
}
