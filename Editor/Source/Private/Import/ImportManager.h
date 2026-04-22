#pragma once

#include "Core/AssetManager.h"
#include "Core/IAssetLoader.h"

#include "Core/SceneReadResult.h"
#include "Import/AssetInfo.h"
#include "Import/Importer.h"

#include <uuid.h>

#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string>

namespace Nightbird::Editor
{
	class ImportManager : public Core::IAssetLoader
	{
	public:
		ImportManager(const std::filesystem::path& assetsDir);

		void Scan();

		const std::filesystem::path& GetAssetsDir() const;

		const std::unordered_map<uuids::uuid, AssetInfo>& GetAssetInfos() const;

		const AssetInfo* GetAssetInfo(const uuids::uuid& uuid) const;
		const AssetInfo* GetAssetInfo(const std::filesystem::path& path) const;

		Importer* FindImporter(const std::filesystem::path& path) const;
		
		std::shared_ptr<Core::Mesh> LoadMesh(const uuids::uuid& uuid) override;
		std::shared_ptr<Core::AudioAsset> LoadAudio(const uuids::uuid& uuid) override;

		Core::SceneReadResult LoadScene(const uuids::uuid& uuid, Core::AssetManager* assetManager = nullptr);

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
