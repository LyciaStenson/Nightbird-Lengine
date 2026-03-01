#pragma once

#include "Core/SpatialObject.h"

#include <toml.hpp>

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
		std::unique_ptr<Core::SpatialObject> Import(const std::filesystem::path& sourcePath);

	private:
		std::filesystem::path m_AssetsDir;
		std::filesystem::path m_CookedDir;
		std::unordered_map<std::string, AssetInfo> m_AssetInfos;
		std::vector<std::unique_ptr<Importer>> m_Importers;

		std::string GenerateUUID() const;

		std::string FindImporter(const std::filesystem::path& sourcePath);
		
		void GenerateAssetInfoFile(const std::filesystem::path& sourcePath);
		void ReadAssetInfoFile(const std::filesystem::path& assetInfoPath);
	};
}