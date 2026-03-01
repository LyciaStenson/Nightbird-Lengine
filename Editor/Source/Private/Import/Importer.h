#pragma once

#include "Core/SpatialObject.h"

#include <string>
#include <memory>

namespace Nightbird::Editor
{
	struct AssetInfo;

	class Importer
	{
	public:
		virtual ~Importer() = default;
		virtual std::string GetName() const = 0;
		virtual bool SupportsExtension(const std::string& extensions) const = 0;
		virtual std::unique_ptr<Core::SpatialObject> Import(const AssetInfo& assetInfo) = 0;
	};
}