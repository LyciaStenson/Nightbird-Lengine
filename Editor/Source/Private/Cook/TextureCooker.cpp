#include "Cook/TextureCooker.h"

#include "Cook/BinaryWriter.h"

#include "Core/Texture.h"
#include "Core/Log.h"

namespace Nightbird::Editor
{
	void TextureCooker::Cook(const Core::Texture& texture, const uuids::uuid& uuid, const std::filesystem::path& outputDir, Endianness endianness)
	{
		std::filesystem::create_directories(outputDir);

		std::filesystem::path outputPath = outputDir / (uuids::to_string(uuid) + ".nbtexture");

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

		// Pixels
		const auto& pixels = texture.GetPixels();
		writer.WriteRawBytes(pixels.data(), pixels.size());

		Core::Log::Info("Cooked texture: " + outputPath.string());
	}
}