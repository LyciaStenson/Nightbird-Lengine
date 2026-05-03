#pragma once

#include "Import/CubemapImporter.h"

#include <uuid.h>

#include <memory>
#include <filesystem>

namespace Nightbird::Core
{
	class Cubemap;
}

namespace Nightbird::Editor
{
	class TextCubemapReader
	{
	public:
		CubemapReadResult Read(const std::filesystem::path& path);
	};
}