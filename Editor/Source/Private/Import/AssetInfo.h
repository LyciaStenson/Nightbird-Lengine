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
		
		std::filesystem::path sourcePath;
		std::string importer;
	};
}