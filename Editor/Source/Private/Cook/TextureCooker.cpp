#include "Cook/TextureCooker.h"

#include "Cook/BinaryWriter.h"

#include "Core/Texture.h"
#include "Core/Log.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace Nightbird::Editor
{
	static bool WritePNG(const std::filesystem::path& path, uint32_t width, uint32_t height, const std::vector<uint8_t>& pixels)
	{
		int result = stbi_write_png(path.string().c_str(), width, height, 4, pixels.data(), width * 4);
		return result != 0;
	}

	void TextureCooker::Cook(const Core::Texture& texture, const uuids::uuid& uuid, const std::filesystem::path& outputDir, CookTarget target, Endianness endianness)
	{
		std::filesystem::create_directories(outputDir);

		std::filesystem::path outputPath = outputDir / (uuids::to_string(uuid) + ".nbtexture");

		Core::TextureFormat format;
		std::vector<uint8_t> data;

		switch (target)
		{
		case CookTarget::Desktop:
			// Fall through
		case CookTarget::WiiU:
			format = Core::TextureFormat::RGBA8;
			data = CookRGBA(texture);
			break;
		case CookTarget::N3DS:
			format = Core::TextureFormat::T3X;
			data = Cook3DS(texture, uuid);
			break;
		default:
			format = Core::TextureFormat::RGBA8;
			data = CookRGBA(texture);
			break;
		}

		BinaryWriter writer(outputPath, endianness);

		// Type
		writer.WriteUInt8('T');
		writer.WriteUInt8('E');
		writer.WriteUInt8('X');
		writer.WriteUInt8('T');

		// Verison
		writer.WriteUInt32(1);

		// Dimensions
		writer.WriteUInt32(texture.GetWidth());
		writer.WriteUInt32(texture.GetHeight());

		// Format
		writer.WriteUInt32(static_cast<uint32_t>(format));

		// Data size
		writer.WriteUInt32(static_cast<uint32_t>(data.size()));

		// Pixels
		writer.WriteRawBytes(data.data(), data.size());

		Core::Log::Info("Cooked texture: " + outputPath.string());
	}

	std::vector<uint8_t> TextureCooker::CookRGBA(const Core::Texture& texture)
	{
		const auto& data = texture.GetData();
		return std::vector<uint8_t>(data.begin(), data.end());
	}

	std::vector<uint8_t> TextureCooker::Cook3DS(const Core::Texture& texture, const uuids::uuid& uuid)
	{
		std::filesystem::path tempDir = std::filesystem::temp_directory_path();

		std::filesystem::path inputPath = tempDir / (uuids::to_string(uuid) + ".png");
		std::filesystem::path outputPath = tempDir / (uuids::to_string(uuid) + ".t3x");

		const auto& srcPixels = texture.GetData();
		std::vector<uint8_t> pixels = srcPixels;

		for (size_t i = 0; i < pixels.size(); i += 4)
		{
			// Force opaque
			pixels[i + 3] = 255;
		}

		Core::Log::Info("Cook3DS: " + std::to_string(texture.GetWidth()) + "x" + std::to_string(texture.GetHeight()));

		if (!WritePNG(inputPath, texture.GetWidth(), texture.GetHeight(), pixels))
		{
			Core::Log::Error("Failed to write temporator PNG for tex3ds.");
			return {};
		}

		std::string command = "tex3ds \"" + inputPath.string() + "\" -f rgba5551 -z auto -o \"" + outputPath.string() + "\"";

		int result = std::system(command.c_str());
		if (result != 0)
		{
			Core::Log::Error("tex3ds failed.");
			return {};
		}

		std::ifstream file(outputPath, std::ios::binary);
		if (!file)
		{
			Core::Log::Error("Failed to open tex3ds output.");
			return {};
		}

		std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		std::filesystem::remove(inputPath);
		std::filesystem::remove(outputPath);
		return data;
	}
}
