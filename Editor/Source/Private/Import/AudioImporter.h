#pragma once

#include "Import/Importer.h"

namespace Nightbird::Editor
{
	class AudioImporter : public Importer
	{
	public:
		std::string GetName() const override;
		bool SupportsExtension(const std::string& extension) const override;
		std::unique_ptr<Core::SceneObject> Import(const AssetInfo& assetInfo) override;
	};
}
