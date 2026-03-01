#pragma once

#include <toml.hpp>

#include <string>
#include <filesystem>

namespace Nightbird::Editor
{
	struct AssetInfo
	{
		std::string uuid;
		std::string importer;
		std::filesystem::path sourcePath;
		toml::table params;
	};
}