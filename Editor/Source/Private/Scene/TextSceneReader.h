#pragma once

#include "Core/SceneReadResult.h"

#include <uuid.h>
#include <toml.hpp>
#include <rttr/variant.h>

#include <memory>
#include <filesystem>

namespace Nightbird::Editor
{
	class TextSceneReader
	{
	public:
		Core::SceneReadResult Read(const std::filesystem::path& path);

	private:
		void DeserializeToml(const toml::table& table, rttr::instance instance);
		rttr::variant TableToVariant(const toml::table& table);
	};
}
