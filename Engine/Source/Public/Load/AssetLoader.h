#pragma once

#include <uuid.h>

#include <filesystem>
#include <memory>
#include <unordered_map>

namespace Nightbird::Core
{
	class Scene;
	class Mesh;
}

namespace Nightbird::Load
{
	class BinarySceneReader;
	class TextureLoader;
	class MaterialLoader;
	class MeshLoader;

	class AssetLoader
	{
	public:
		AssetLoader(const std::filesystem::path& cookedDir);
		
		std::unique_ptr<Core::Scene> LoadScene(const uuids::uuid& uuid);

		std::shared_ptr<Core::Mesh> LoadMesh(const uuids::uuid& uuid);

	private:
		std::filesystem::path m_CookedDir;

		std::unique_ptr<BinarySceneReader> m_SceneReader;
		std::unique_ptr<TextureLoader> m_TextureLoader;
		std::unique_ptr<MaterialLoader> m_MaterialLoader;
		std::unique_ptr<MeshLoader> m_MeshLoader;

		std::unordered_map<uuids::uuid, std::shared_ptr<Core::Mesh>> m_MeshCache;
	};
}