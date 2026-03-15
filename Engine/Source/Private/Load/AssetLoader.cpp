#include "Load/AssetLoader.h"

#include "Scene/BinarySceneReader.h"

#include "Load/TextureLoader.h"
#include "Load/MaterialLoader.h"
#include "Load/MeshLoader.h"

#include "Core/Scene.h"
#include "Core/Log.h"

namespace Nightbird::Load
{
	AssetLoader::AssetLoader(const std::string& cookedDir)
		: m_CookedDir(cookedDir)
	{
		m_SceneReader = std::make_unique<BinarySceneReader>(*this);
		m_TextureLoader = std::make_unique<TextureLoader>();
		m_MaterialLoader = std::make_unique<MaterialLoader>(*m_TextureLoader);
		m_MeshLoader = std::make_unique<MeshLoader>(*m_MaterialLoader);
	}

	std::shared_ptr<Core::Mesh> AssetLoader::LoadMesh(const uuids::uuid& uuid)
	{
		auto it = m_MeshCache.find(uuid);
		if (it != m_MeshCache.end())
			return it->second;

		auto mesh = m_MeshLoader->Load(m_CookedDir, uuid);
		if (!mesh)
		{
			Core::Log::Error("AssetLoader: Failed to load mesh: " + uuids::to_string(uuid));
			return nullptr;
		}

		m_MeshCache[uuid] = mesh;
		return mesh;
	}
	
	std::unique_ptr<Core::Scene> AssetLoader::LoadScene(const uuids::uuid& uuid)
	{
		return m_SceneReader->ReadScene(m_CookedDir, uuid);
	}
}
