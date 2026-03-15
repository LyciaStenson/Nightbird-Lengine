#include "Load/TextureLoader.h"

#include "Load/BinaryReader.h"

#include "Core/Texture.h"
#include "Core/Log.h"

#include <cstdint>

namespace Nightbird::Load
{
	std::shared_ptr<Core::Texture> TextureLoader::Load(const std::string& cookedDir, const uuids::uuid& uuid)
	{
		std::string path = cookedDir + "/" + uuids::to_string(uuid) + ".nbtexture";

		BinaryReader reader(path);
		if (!reader.IsValid())
		{
			Core::Log::Error("TextureLoader: Failed to open: " + path);
			return nullptr;
		}

		// Validate Type
		uint8_t type[4] = {};
		reader.ReadRawBytes(type, 4);
		if (type[0] != 'T' || type[1] != 'E' || type[2] != 'X' || type[3] != 'T')
		{
			Core::Log::Error("TextureLoader: Invalid type signature in: " + path);
			return nullptr;
		}

		// Check Version
		uint32_t version = reader.ReadUInt32();
		if (version != 1)
		{
			Core::Log::Error("TextureLoader: Unsupported version: " + std::to_string(version));
			return nullptr;
		}

		uint32_t width = reader.ReadUInt32();
		uint32_t height = reader.ReadUInt32();
		uint32_t pixelCount = width * height * 4;
		std::vector<uint8_t> pixels(pixelCount);
		reader.ReadRawBytes(pixels.data(), pixelCount);

		auto texture = std::make_shared<Core::Texture>(width, height, std::move(pixels));
		return texture;
	}
}
