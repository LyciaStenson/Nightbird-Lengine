#pragma once

#include "Cook/Endianness.h"

#include <uuid.h>

#include <filesystem>

namespace Nightbird::Core
{
	class Texture;
}

namespace Nightbird::Editor
{
	class TextureCooker
	{
	public:
		void Cook(const Core::Texture& texture, const uuids::uuid& uuid, const std::filesystem::path& outputDir, Endianness endianness);
	};
}