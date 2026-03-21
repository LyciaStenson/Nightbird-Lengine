#include "Load/AudioLoader.h"

#include "Load/BinaryReader.h"

#include "Core/AudioAsset.h"
#include "Core/Log.h"

namespace Nightbird::Load
{
	std::shared_ptr<Core::AudioAsset> AudioLoader::Load(const std::string& cookedDir, const uuids::uuid& uuid)
	{
		std::string path = cookedDir + "/" + uuids::to_string(uuid) + ".nbaudio";
		BinaryReader reader(path);
		if (!reader.IsValid())
		{
			Core::Log::Error("AudioLoader: Failed to open: " + path);
			return nullptr;
		}

		// Validate type
		uint8_t type[4] = {};
		reader.ReadRawBytes(type, 4);
		if (type[0] != 'A' || type[1] != 'D' || type[2] != 'I' || type[3] != 'O')
		{
			Core::Log::Error("AudioLoader: Invalid type signature in: " + path);
			return nullptr;
		}

		// Version
		uint32_t version = reader.ReadUInt32();
		if (version != 1)
		{
			Core::Log::Error("AudioLoader: Unsupported version: " + std::to_string(version));
			return nullptr;
		}

		// Encoding, channels, padding
		Core::AudioEncoding encoding = static_cast<Core::AudioEncoding>(reader.ReadUInt8());
		uint8_t channels = reader.ReadUInt8();
		reader.ReadUInt16(); // Padding

		// Sample rate, frame count
		uint32_t sampleRate = reader.ReadUInt32();
		uint32_t frameCount = reader.ReadUInt32();

		// Channel data size
		std::vector<uint32_t> channelSizes(channels);
		for (uint8_t channel = 0; channel < channels; channel++)
			channelSizes[channel] = reader.ReadUInt32();

		// Channel data
		std::vector<std::vector<uint8_t>> channelData(channels);
		for (uint8_t channel = 0; channel < channels; channel++)
		{
			channelData[channel].resize(channelSizes[channel]);
			reader.ReadRawBytes(channelData[channel].data(), channelSizes[channel]);
		}

		return std::make_shared<Core::AudioAsset>(sampleRate, frameCount, channels, encoding, std::move(channelData));
	}
}
