#pragma once

#include <uuid.h>

#include <memory>
#include <filesystem>

namespace Nightbird::Core
{
	class Texture;
}

namespace Nightbird::Load
{
	class TextureLoader
	{
	public:
		std::shared_ptr<Core::Texture> Load(const std::filesystem::path& cookedDir, const uuids::uuid& uuid);
	};
}