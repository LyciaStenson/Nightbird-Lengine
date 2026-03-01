#include "Import/ImportManager.h"

#include "Import/AssetInfo.h"
#include "Import/GltfImporter.h"

#include "Core/Log.h"

#include <random>

namespace Nightbird::Editor
{
	ImportManager::ImportManager(const std::filesystem::path& assetsDir)
		: m_AssetsDir(assetsDir)
	{
		m_Importers.push_back(std::make_unique<GltfImporter>());
	}

	void ImportManager::Scan()
	{
		if (!std::filesystem::exists(m_AssetsDir))
		{
			Core::Log::Warning("Assets directory does not exist: " + m_AssetsDir.string());
			return;
		}

		for (const auto& entry : std::filesystem::recursive_directory_iterator(m_AssetsDir))
		{
			if (!entry.is_regular_file())
				continue;

			if (entry.path().extension() == ".assetinfo")
				continue;

			std::filesystem::path assetInfoPath = entry.path().string() + ".assetinfo";

			if (!std::filesystem::exists(assetInfoPath))
				GenerateAssetInfoFile(entry.path());
			else
				ReadAssetInfoFile(assetInfoPath);
		}
	}

	std::unique_ptr<Core::SceneObject> ImportManager::Import(const std::filesystem::path& sourcePath)
	{
		std::filesystem::path infoPath = sourcePath.string() + ".assetInfo";
		if (!std::filesystem::exists(infoPath))
		{
			Core::Log::Warning("No .assetinfo file found for " + sourcePath.string());
			return nullptr;
		}

		ReadAssetInfoFile(infoPath);

		AssetInfo* foundAsset = nullptr;
		for (auto& [uuid, assetInfo] : m_AssetInfos)
		{
			if (assetInfo.sourcePath == sourcePath)
			{
				foundAsset = &assetInfo;
				break;
			}
		}

		if (!foundAsset)
		{
			Core::Log::Warning("Failed to find asser info for: " + sourcePath.string());
			return nullptr;
		}

		for (const auto& importer : m_Importers)
		{
			if (importer->GetName() == foundAsset->importer)
				return importer->Import(*foundAsset);
		}

		Core::Log::Warning("No importer found for: " + foundAsset->importer);
		return nullptr;
	}

	std::string ImportManager::GenerateUUID() const
	{
		std::random_device randomDevice;
		std::mt19937_64 gen(randomDevice());
		std::uniform_int_distribution<uint64_t> dis;

		uint64_t high = dis(gen);
		uint64_t low = dis(gen);

		high = (high & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
		low = (low & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;

		std::ostringstream stream;
		stream << std::hex << std::setfill('0');
		stream << std::setw(8) << ((high >> 32) & 0xFFFFFFFF) << '-';
		stream << std::setw(4) << ((high >> 16) & 0xFFFF) << '-';
		stream << std::setw(4) << (high & 0xFFFF) << '-';
		stream << std::setw(4) << ((low >> 48) & 0xFFFF) << '-';
		stream << std::setw(12) << (low & 0xFFFFFFFFFFFFULL);

		return stream.str();
	}

	std::string ImportManager::FindImporter(const std::filesystem::path& sourcePath)
	{
		for (const auto& importer : m_Importers)
		{
			if (importer->SupportsExtension(sourcePath.extension().string()))
				return importer->GetName();
		}
		return {};
	}

	void ImportManager::GenerateAssetInfoFile(const std::filesystem::path& sourcePath)
	{
		std::string importerName = FindImporter(sourcePath);
		if (importerName.empty())
			return;

		std::string uuid = GenerateUUID();

		toml::table info;
		info.insert("uuid", uuid);
		info.insert("importer", importerName);
		info.insert("importer_version", 1);
		info.insert("source_file", sourcePath.string());

		toml::table table;
		table.insert("info", info);
		table.insert("params", toml::table{});

		std::filesystem::path assetInfoPath = sourcePath.string() + ".assetinfo";
		std::ofstream file(assetInfoPath);
		file << table;
		file.close();

		Core::Log::Info("Generated .assetinfo file: " + assetInfoPath.string());

		ReadAssetInfoFile(assetInfoPath);
	}

	void ImportManager::ReadAssetInfoFile(const std::filesystem::path& assetInfoPath)
	{
		if (!std::filesystem::exists(assetInfoPath) || std::filesystem::file_size(assetInfoPath) == 0)
		{
			Core::Log::Error("Invalid .assetinfo file: " + assetInfoPath.string());
			return;
		}

		toml::table table = toml::parse_file(assetInfoPath.string());
		
		AssetInfo assetInfo;
		assetInfo.uuid = table["info"]["uuid"].value_or(std::string{});
		assetInfo.importer = table["info"]["importer"].value_or(std::string{});
		assetInfo.sourcePath = table["info"]["source_file"].value_or(std::string{});
		
		if (table.contains("params"))
			assetInfo.params = *table["params"].as_table();

		if (assetInfo.uuid.empty())
		{
			Core::Log::Warning("Missing UUID in: " + assetInfoPath.string());
			return;
		}

		m_AssetInfos[assetInfo.uuid] = std::move(assetInfo);
	}
}