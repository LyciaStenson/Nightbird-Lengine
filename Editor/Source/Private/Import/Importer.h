#pragma once

#include <string>

namespace Nightbird::Editor
{
	struct ImportedAsset;

	class Importer
	{
	public:
		virtual ~Importer() = default;
		virtual std::string GetName() = 0;
		virtual bool SupportsExtension(const std::string& extensions) const = 0;
		virtual void Import(const ImportedAsset& asset) = 0;
	};
}