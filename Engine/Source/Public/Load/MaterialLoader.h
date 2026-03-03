#pragma once

#include <uuid.h>

#include <memory>
#include <filesystem>
#include <unordered_map>

namespace Nightbird::Core
{
	struct Material;
	class Texture;
}

namespace Nightbird::Load
{
	class TextureLoader;

	class MaterialLoader
	{
	public:
		MaterialLoader(TextureLoader& textureLoader);

		std::shared_ptr<Core::Material> Load(const std::filesystem::path& cookedDir, const uuids::uuid& uuid);

	private:
		TextureLoader& m_TextureLoader;
		std::unordered_map<uuids::uuid, std::shared_ptr<Core::Texture>> m_TextureCache;

		std::shared_ptr<Core::Texture> LoadTexture(const std::filesystem::path& cookedDir, const uuids::uuid& uuid);
	};
}