#pragma once

#include "Import/SceneImporter.h"

#include "Core/SceneObject.h"

namespace Nightbird::Editor
{
	class TextSceneImporter : public SceneImporter
	{
	public:
		std::string GetName() const override { return "TextScene"; }
		bool SupportsExtension(const std::string& extension) const override;
		bool HasEmbeddedAssetInfo() const override { return true; }
		std::optional<AssetInfo> ReadEmbeddedAssetInfo(const std::filesystem::path& path) const override;

		Core::SceneReadResult Load(const AssetInfo& assetInfo) override;
	};
}
