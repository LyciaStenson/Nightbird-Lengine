#pragma once

#include "Core/SceneInstance.h"

#include <uuid.h>

#include <filesystem>
#include <unordered_map>
#include <string>

namespace Nightbird::Editor
{
	struct AssetInfo;
	class Importer;

	class ImportManager
	{
	public:
		ImportManager(const std::filesystem::path& assetsDir);

		void Scan();
		std::unique_ptr<Core::SceneInstance> Import(const std::filesystem::path& sourcePath);

		const AssetInfo* GetAssetInfo(const uuids::uuid& uuid) const;

	private:
		std::filesystem::path m_AssetsDir;
		std::filesystem::path m_CookedDir;
		std::unordered_map<uuids::uuid, AssetInfo> m_AssetInfos;
		std::vector<std::unique_ptr<Importer>> m_Importers;

		uuids::uuid GenerateUUID() const;

		std::string FindImporter(const std::filesystem::path& sourcePath);
		
		void GenerateAssetInfoFile(const std::filesystem::path& sourcePath);
		void ReadAssetInfoFile(const std::filesystem::path& assetInfoPath);
	};
}