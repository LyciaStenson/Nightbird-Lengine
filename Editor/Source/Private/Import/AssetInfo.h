#pragma once

#include <uuid.h>
#include <toml.hpp>

#include <string>
#include <filesystem>

namespace Nightbird::Editor
{
	struct AssetInfo
	{
		uuids::uuid uuid;
		std::string importer;
		std::filesystem::path sourcePath;
		toml::table params;
	};
}