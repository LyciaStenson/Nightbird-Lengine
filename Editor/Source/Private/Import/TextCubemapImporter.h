#pragma once

#include "Import/CubemapImporter.h"

namespace Nightbird::Editor
{
	class TextCubemapImporter : public CubemapImporter
	{
	public:
		std::string GetName() const override { return "TextCubemap"; }
		bool SupportsExtension(const std::string& extension) const override;
		bool HasEmbeddedAssetInfo() const override { return true; }
		std::optional<AssetInfo> ReadEmbeddedAssetInfo(const std::filesystem::path& path) const override;

		CubemapReadResult Load(const AssetInfo& assetInfo, Core::AssetManager* assetManager = nullptr) override;
	};
}
