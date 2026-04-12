#pragma once

#include <uuid.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace Nightbird::Core
{
	struct Material;
	class Texture;

	class TextureLoader;

	class MaterialLoader
	{
	public:
		MaterialLoader(TextureLoader& textureLoader);

		std::shared_ptr<Material> Load(const std::string& cookedDir, const uuids::uuid& uuid);

	private:
		TextureLoader& m_TextureLoader;
		std::unordered_map<uuids::uuid, std::shared_ptr<Texture>> m_TextureCache;

		std::shared_ptr<Texture> LoadTexture(const std::string& cookedDir, const uuids::uuid& uuid);
	};
}
