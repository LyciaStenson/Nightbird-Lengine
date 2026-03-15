#pragma once

#include <uuid.h>

#include <memory>
#include <string>

namespace Nightbird::Core
{
	class Texture;
}

namespace Nightbird::Load
{
	class TextureLoader
	{
	public:
		std::shared_ptr<Core::Texture> Load(const std::string& cookedDir, const uuids::uuid& uuid);
	};
}
