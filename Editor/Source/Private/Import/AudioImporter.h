#pragma once

#include "Import/Importer.h"

#include <memory>

namespace Nightbird::Core
{
	class AudioAsset;
}

namespace Nightbird::Editor
{
	struct AssetInfo;

	class AudioImporter : public Importer
	{
	public:
		AssetType GetAssetType() const override { return AssetType::Audio; }
		virtual std::shared_ptr<Core::AudioAsset> Load(const AssetInfo& assetInfo) = 0;
	};
}
