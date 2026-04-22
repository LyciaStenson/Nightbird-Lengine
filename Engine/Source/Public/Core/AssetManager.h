#pragma once

#include "Core/IAssetLoader.h"

#include <uuid.h>

#include <memory>

namespace Nightbird::Core
{
	class AssetManager
	{
	public:
		explicit AssetManager(IAssetLoader& loader)
			: m_Loader(loader) {}
		
		template<typename T>
		std::weak_ptr<T> Load(const uuids::uuid& uuid)
		{
			auto it = m_Cache.find(uuid);
			if (it != m_Cache.end())
				return std::static_pointer_cast<T>(it->second);

			std::shared_ptr<T> asset = LoadInternal<T>(uuid);
			if (asset)
				m_Cache[uuid] = asset;
			return asset;
		}

		void Unload(const uuids::uuid& uuid)
		{
			m_Cache.erase(uuid);
		}

		void UnloadAll()
		{
			m_Cache.clear();
		}

	private:
		template<typename T>
		std::shared_ptr<T> LoadInternal(const uuids::uuid& uuid)
		{
			if constexpr (std::is_same_v<T, Mesh>)
				return m_Loader.LoadMesh(uuid);
			if constexpr (std::is_same_v<T, AudioAsset>)
				return m_Loader.LoadAudio(uuid);
			else
				return nullptr;
		}

		IAssetLoader& m_Loader;
		std::unordered_map<uuids::uuid, std::shared_ptr<void>> m_Cache;
	};
}