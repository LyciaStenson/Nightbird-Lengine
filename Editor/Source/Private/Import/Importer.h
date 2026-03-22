#pragma once

#include "Import/AssetType.h"

#include <string>

namespace Nightbird::Editor
{
	class Importer
	{
	public:
		virtual ~Importer() = default;
		virtual std::string GetName() const = 0;
		virtual bool SupportsExtension(const std::string& extensions) const = 0;
		virtual AssetType GetAssetType() const = 0;
	};
}
