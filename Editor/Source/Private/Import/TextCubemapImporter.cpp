#include "Import/TextCubemapImporter.h"

#include "Import/TextCubemapReader.h"

#include "Core/Log.h"

#include <toml.hpp>

namespace Nightbird::Editor
{
	bool TextCubemapImporter::SupportsExtension(const std::string& extension) const
	{
		return extension == ".ntcubemap";
	}

	std::optional<AssetInfo> TextCubemapImporter::ReadEmbeddedAssetInfo(const std::filesystem::path& path) const
	{
		Core::Log::Info("ReadEmbeddedAssetInfo");

		if (!std::filesystem::exists(path))
		{
			Core::Log::Error("TextCubemapImporter: File not found: " + path.string());
			return std::nullopt;
		}

		toml::parse_result tomlResult = toml::parse_file(path.string());
		if (!tomlResult)
		{
			Core::Log::Error("TextCubemapImporter: Failed to parse: " + path.string());
			return std::nullopt;
		}

		toml::table& document = tomlResult.table();
		
		std::string cubemapUUIDString = document["cubemap"]["uuid"].value_or(std::string{});
		auto cubemapUUID = uuids::uuid::from_string(cubemapUUIDString);
		if (!cubemapUUID)
		{
			Core::Log::Error("TextCubemapImporter: Invalid cubemap UUID in: " + path.string());
			return std::nullopt;
		}

		AssetInfo assetInfo;
		assetInfo.uuid = *cubemapUUID;
		assetInfo.importer = GetName();
		assetInfo.sourcePath = path;

		return assetInfo;
	}
	
	CubemapReadResult TextCubemapImporter::Load(const AssetInfo& assetInfo, Core::AssetManager* assetManager)
	{
		TextCubemapReader reader;
		return reader.Read(assetInfo.sourcePath);
	}
}
