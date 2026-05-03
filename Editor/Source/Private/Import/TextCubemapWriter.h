#pragma once

#include <uuid.h>

#include <filesystem>

namespace Nightbird::Core
{
	class Cubemap;
}

namespace Nightbird::Editor
{
	class TextCubemapWriter
	{
	public:
		void Write(const Core::Cubemap& cubemap, const uuids::uuid& cubemapUUID, const std::filesystem::path& outputPath);
	};
}