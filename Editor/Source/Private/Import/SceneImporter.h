#pragma once

#include "Import/Importer.h"

#include <memory>

namespace Nightbird::Core
{
	class SceneObject;
}

namespace Nightbird::Editor
{
	struct AssetInfo;

	class SceneImporter : public Importer
	{
	public:
		AssetType GetAssetType() const override { return AssetType::Scene; }
		virtual std::unique_ptr<Core::SceneObject> Load(const AssetInfo& assetInfo) = 0;
	};
}
