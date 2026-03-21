#pragma once

#include <uuid.h>

#include <memory>
#include <string>

namespace Nightbird::Core
{
	class AudioAsset;
}

namespace Nightbird::Load
{
	class AudioLoader
	{
	public:
		std::shared_ptr<Core::AudioAsset> Load(const std::string& cookedDir, const uuids::uuid& uuid);
	};
}
