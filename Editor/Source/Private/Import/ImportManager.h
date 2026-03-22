#pragma once

#include "Import/AssetType.h"

#include <uuid.h>

#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string>

namespace Nightbird::Core
{
	class SceneObject;
	class AudioAsset;
}

namespace Nightbird::Editor
{
	struct AssetInfo;
	class Importer;

	class ImportManager
	{
	public:
		ImportManager(const std::filesystem::path& assetsDir);

		void Scan();

		AssetType GetAssetType(const uuids::uuid& uuid) const;
		const AssetInfo* GetAssetInfo(const uuids::uuid& uuid) const;

		std::unique_ptr<Core::SceneObject> LoadScene(const uuids::uuid& sourcePath);
		std::shared_ptr<Core::AudioAsset> LoadAudio(const uuids::uuid& sourcePath);

	private:
		std::filesystem::path m_AssetsDir;
		std::unordered_map<uuids::uuid, AssetInfo> m_AssetInfos;
		std::vector<std::unique_ptr<Importer>> m_Importers;

		uuids::uuid GenerateUUID() const;

		std::string FindImporterName(const std::filesystem::path& sourcePath);

		void GenerateAssetInfoFile(const std::filesystem::path& sourcePath);
		void ReadAssetInfoFile(const std::filesystem::path& assetInfoPath);
	};
}
