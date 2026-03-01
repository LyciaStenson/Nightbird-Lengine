#pragma once

#include <toml.hpp>

#include <filesystem>
#include <unordered_map>
#include <string>

namespace Nightbird::Editor
{
	struct ImportedAsset
	{
		std::string uuid;
		std::string importer;
		std::filesystem::path sourcePath;
		toml::table params;
	};

	class Importer;

	class ImportManager
	{
	public:
		ImportManager(const std::filesystem::path& assetsDir);

		void Scan();
		void Import(const std::filesystem::path& sourcePath);

	private:
		std::filesystem::path m_AssetsDir;
		std::filesystem::path m_CookedDir;
		std::unordered_map<std::string, ImportedAsset> m_Assets;
		std::vector<std::unique_ptr<Importer>> m_Importers;
	};
}