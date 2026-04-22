#include "Core/AssetLoader.h"

#include "Core/Scene.h"
#include "Core/Mesh.h"
#include "Core/AudioAsset.h"
#include "Core/Log.h"

namespace Nightbird::Core
{
	AssetLoader::AssetLoader(const std::string& cookedDir)
		: m_CookedDir(cookedDir)
	{
		m_SceneReader = std::make_unique<BinarySceneReader>();
		m_ProjectLoader = std::make_unique<ProjectLoader>();
		m_TextureLoader = std::make_unique<TextureLoader>();
		m_MaterialLoader = std::make_unique<MaterialLoader>(*m_TextureLoader);
		m_MeshLoader = std::make_unique<MeshLoader>(*m_MaterialLoader);
		m_AudioLoader = std::make_unique<AudioLoader>();
	}

	ProjectInfo AssetLoader::LoadProject()
	{
		return m_ProjectLoader->Load(m_CookedDir);
	}
	
	std::shared_ptr<Mesh> AssetLoader::LoadMesh(const uuids::uuid& uuid)
	{
		return m_MeshLoader->Load(m_CookedDir, uuid);
	}
	
	std::shared_ptr<AudioAsset> AssetLoader::LoadAudio(const uuids::uuid& uuid)
	{
		return m_AudioLoader->Load(m_CookedDir, uuid);
	}

	SceneReadResult AssetLoader::LoadScene(const uuids::uuid& uuid)
	{
		SceneReadResult result = m_SceneReader->Read(m_CookedDir, uuid);
		if (!result.root)
		{
			Log::Error("AssetLoader: Failed to read scene: " + uuids::to_string(uuid));
			return result;
		}

		for (auto& child : result.root->GetChildren())
			LoadNestedScenes(child.get());
		
		return result;
	}

	void AssetLoader::LoadNestedScenes(SceneObject* object)
	{
		if (!object)
			return;

		if (const auto& sceneUUID = object->GetSourceSceneUUID())
		{
			SceneReadResult nested = LoadScene(*sceneUUID);

			if (nested.root)
			{
				for (auto& child : nested.root->GetChildren())
					object->AddChild(std::move(child));
			}
			else
			{
				Log::Warning("AssetLoader: Failed to load nested scene: " + uuids::to_string(*sceneUUID));
			}

			return; // Don't recurse into children of nested scene
		}

		for (auto& child : object->GetChildren())
			LoadNestedScenes(child.get());
	}
}
