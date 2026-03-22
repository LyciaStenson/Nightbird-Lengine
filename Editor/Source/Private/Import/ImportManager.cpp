#include "Import/ImportManager.h"

#include "Import/AssetInfo.h"
#include "Import/AssetType.h"
#include "Import/GltfSceneImporter.h"
#include "Import/DrLibsAudioImporter.h"

#include "Core/Log.h"

#include <toml.hpp>

#include <random>

namespace Nightbird::Editor
{
	ImportManager::ImportManager(const std::filesystem::path& assetsDir)
		: m_AssetsDir(assetsDir)
	{
		m_Importers.push_back(std::make_unique<GltfSceneImporter>());
		m_Importers.push_back(std::make_unique<DrLibsAudioImporter>());
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

	AssetType ImportManager::GetAssetType(const uuids::uuid& uuid) const
	{
		const AssetInfo* assetInfo = GetAssetInfo(uuid);
		if (!assetInfo)
			return AssetType::Unknown;

		for (const auto& importer : m_Importers)
		{
			if (importer->GetName() == assetInfo->importer)
				return importer->GetAssetType();
		}
		return AssetType::Unknown;
	}

	const AssetInfo* ImportManager::GetAssetInfo(const uuids::uuid& uuid) const
	{
		auto it = m_AssetInfos.find(uuid);
		if (it != m_AssetInfos.end())
			return &it->second;
		return nullptr;
	}

	std::unique_ptr<Core::SceneObject> ImportManager::LoadScene(const uuids::uuid& uuid)
	{
		const AssetInfo* assetInfo = GetAssetInfo(uuid);
		if (!assetInfo)
		{
			Core::Log::Warning("ImportManager: AssetInfo not found for scene UUID: " + uuids::to_string(uuid));
			return nullptr;
		}

		for (const auto& importer : m_Importers)
		{
			if (importer->GetName() == assetInfo->importer)
			{
				if (importer->GetAssetType() == AssetType::Scene)
					return static_cast<SceneImporter*>(importer.get())->Load(*assetInfo);
			}
		}

		Core::Log::Warning("ImportManager: No scene importer found for: " + assetInfo->importer);
		return nullptr;
	}

	std::shared_ptr<Core::AudioAsset> ImportManager::LoadAudio(const uuids::uuid& uuid)
	{
		const AssetInfo* assetInfo = GetAssetInfo(uuid);
		if (!assetInfo)
		{
			Core::Log::Warning("ImportManager: AssetInfo not found for audio UUID: " + uuids::to_string(uuid));
			return nullptr;
		}

		for (const auto& importer : m_Importers)
		{
			if (importer->GetName() == assetInfo->importer)
			{
				if (importer->GetAssetType() == AssetType::Audio)
					return static_cast<AudioImporter*>(importer.get())->Load(*assetInfo);
			}
		}

		Core::Log::Warning("ImportManager: No audio importer found for: " + assetInfo->importer);
		return nullptr;
	}

	uuids::uuid ImportManager::GenerateUUID() const
	{
		std::random_device randomDevice;

		auto seedData = std::array<int, std::mt19937::state_size>{};
		std::generate(std::begin(seedData), std::end(seedData), std::ref(randomDevice));
		std::seed_seq seq(std::begin(seedData), std::end(seedData));
		std::mt19937 generator(seq);
		uuids::uuid_random_generator gen{generator};

		return gen();
	}

	std::string ImportManager::FindImporterName(const std::filesystem::path& sourcePath)
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
		std::string importerName = FindImporterName(sourcePath);
		if (importerName.empty())
			return;

		uuids::uuid uuid = GenerateUUID();

		toml::table info;
		info.insert("uuid", uuids::to_string(uuid));
		info.insert("importer", importerName);
		info.insert("importer_version", 1);

		toml::table table;
		table.insert("info", info);
		table.insert("params", toml::table{});

		std::filesystem::path assetInfoPath = sourcePath.string() + ".assetinfo";
		std::ofstream file(assetInfoPath);
		file << table;
		file.close();

		Core::Log::Info("Generated .assetinfo: " + assetInfoPath.string());

		ReadAssetInfoFile(assetInfoPath);
	}

	void ImportManager::ReadAssetInfoFile(const std::filesystem::path& assetInfoPath)
	{
		if (!std::filesystem::exists(assetInfoPath) || std::filesystem::file_size(assetInfoPath) == 0)
		{
			Core::Log::Error("Invalid .assetinfo: " + assetInfoPath.string());
			return;
		}

		toml::parse_result result = toml::parse_file(assetInfoPath.string());
		if (!result)
		{
			Core::Log::Error("Failed to parse .assetinfo: " + assetInfoPath.string());
			return;
		}
		
		toml::table table = result.table();
		
		std::string uuidString = table["info"]["uuid"].value_or(std::string{});
		auto uuid = uuids::uuid::from_string(uuidString);
		if (!uuid)
		{
			Core::Log::Warning("Invalid UUID in: " + assetInfoPath.string());
			return;
		}

		AssetInfo assetInfo;
		assetInfo.uuid = *uuid;
		assetInfo.importer = table["info"]["importer"].value_or(std::string{});

		std::string assetInfoPathString = assetInfoPath.string();
		assetInfo.sourcePath = assetInfoPathString.substr(0, assetInfoPathString.size() - std::string(".assetinfo").size());

		if (table.contains("params"))
			assetInfo.params = *table["params"].as_table();
		
		m_AssetInfos[assetInfo.uuid] = std::move(assetInfo);
	}
}
