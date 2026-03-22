#include "Cook/AudioCooker.h"

#include "Cook/BinaryWriter.h"

#include "Core/AudioAsset.h"
#include "Core/Log.h"

#include <dr_wav.h>
#include <dr_flac.h>

#include <fstream>
#include <cstdint>

namespace Nightbird::Editor
{
	static std::string GetVGAudioCliPath()
	{
		return "dotnet Tools/VGAudioCli/VGAudioCli.dll";
	}

	static std::vector<uint8_t> ReadFile(const std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file)
			return {};
		return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	}

	void AudioCooker::Cook(const std::filesystem::path& sourcePath, const uuids::uuid& uuid, const std::filesystem::path& outputDir, CookTarget target, Endianness endianness)
	{
		std::filesystem::create_directories(outputDir);
		std::filesystem::path outputPath = outputDir / (uuids::to_string(uuid) + ".nbaudio");

		uint32_t sampleRate = 0;
		uint32_t frameCount = 0;
		uint8_t channels = 0;
		Core::AudioEncoding encoding;
		std::vector<uint8_t> data;

		switch (target)
		{
			case CookTarget::Desktop:
				// Faill through
			case CookTarget::WiiU:
				encoding = Core::AudioEncoding::PCM16;
				data = CookPCM16(sourcePath, sampleRate, frameCount, channels);
				break;
			case CookTarget::N3DS:
				encoding = Core::AudioEncoding::DSP_ADPCM;
				data = CookDSPADPCM(sourcePath, uuid, sampleRate, frameCount, channels);
				break;
			default:
				encoding = Core::AudioEncoding::PCM16;
				data = CookPCM16(sourcePath, sampleRate, frameCount, channels);
				break;
		}

		if (data.empty())
		{
			Core::Log::Error("AudioCooker: Failed to cook: " + sourcePath.string());
			return;
		}

		BinaryWriter writer(outputPath, endianness);

		// Type
		writer.WriteUInt8('A');
		writer.WriteUInt8('D');
		writer.WriteUInt8('I');
		writer.WriteUInt8('O');

		// Version
		writer.WriteUInt32(1);

		// Encoding
		writer.WriteUInt8(static_cast<uint8_t>(encoding));

		// Channels
		writer.WriteUInt8(channels);

		// Padding
		writer.WriteUInt16(0);

		// Sample rate
		writer.WriteUInt32(sampleRate);

		// Frame count
		writer.WriteUInt32(frameCount);

		// Data
		writer.WriteRawBytes(data.data(), data.size());

		Core::Log::Info("Cooked audio: " + outputPath.string());
	}

	std::vector<uint8_t> AudioCooker::CookPCM16(const std::filesystem::path& sourcePath, uint32_t& outSampleRate, uint32_t& outFrameCount, uint8_t& outChannels)
	{
		std::string extension = sourcePath.extension().string();
		for (auto& c : extension)
			c = tolower(c);

		unsigned int channels, sampleRate;

		// Collect metadata
		if (extension == ".wav")
		{
			drwav_uint64 frameCount;

			drwav_int16* decoded = drwav_open_file_and_read_pcm_frames_s16(sourcePath.string().c_str(), &channels, &sampleRate, &frameCount, nullptr);

			if (!decoded)
			{
				Core::Log::Error("AudioCooker: Failed to decode WAV: " + sourcePath.string());
				return {};
			}

			outSampleRate = sampleRate;
			outFrameCount = static_cast<uint32_t>(frameCount);
			outChannels = static_cast<uint8_t>(channels);

			uint32_t dataSize = static_cast<uint32_t>(frameCount * channels * sizeof(drwav_int16));

			std::vector<uint8_t> result;
			result.insert(result.end(), reinterpret_cast<uint8_t*>(&dataSize), reinterpret_cast<uint8_t*>(&dataSize) + 4);
			result.insert(result.end(), reinterpret_cast<const uint8_t*>(decoded), reinterpret_cast<const uint8_t*>(decoded) + dataSize);

			drwav_free(decoded, nullptr);
			return result;
		}
		else if (extension == ".flac")
		{
			drflac_uint64 frameCount;

			drflac_int16* decoded = drflac_open_file_and_read_pcm_frames_s16(sourcePath.string().c_str(), &channels, &sampleRate, reinterpret_cast<drflac_uint64*>(&frameCount), nullptr);

			if (!decoded)
			{
				Core::Log::Error("AudioCooker: Failed to decode FLAC: " + sourcePath.string());
				return {};
			}

			outSampleRate = sampleRate;
			outFrameCount = static_cast<uint32_t>(frameCount);
			outChannels = static_cast<uint8_t>(channels);

			uint32_t dataSize = static_cast<uint32_t>(frameCount * channels * sizeof(drflac_int16));

			std::vector<uint8_t> result;
			result.insert(result.end(), reinterpret_cast<uint8_t*>(&dataSize), reinterpret_cast<uint8_t*>(&dataSize) + 4);
			result.insert(result.end(), reinterpret_cast<const uint8_t*>(decoded), reinterpret_cast<const uint8_t*>(decoded) + dataSize);

			drflac_free(decoded, nullptr);
			return result;
		}

		Core::Log::Error("AudioCooker: Unsupported format for PCM16 cooking: " + extension);
		return {};
	}

	std::vector<uint8_t> AudioCooker::CookDSPADPCM(const std::filesystem::path& sourcePath, const uuids::uuid& uuid, uint32_t& outSampleRate, uint32_t& outFrameCount, uint8_t& outChannels)
	{
		std::string extension = sourcePath.extension().string();
		for (auto& c : extension)
			c = tolower(c);

		std::filesystem::path tempDir = std::filesystem::temp_directory_path();
		std::string uuidString = uuids::to_string(uuid);

		std::filesystem::path wavPath;
		bool tempWav = false;

		if (extension == ".wav")
		{
			drwav wav;
			if (!drwav_init_file(&wav, sourcePath.string().c_str(), nullptr))
			{
				Core::Log::Error("AudioCooker: Failed to open WAV for metadata: " + sourcePath.string());
				return {};
			}

			outSampleRate = wav.sampleRate;
			outFrameCount = static_cast<uint32_t>(wav.totalPCMFrameCount);
			outChannels = static_cast<uint8_t>(wav.channels);

			drwav_uninit(&wav);

			wavPath = sourcePath;
		}
		else if (extension == ".flac")
		{
			unsigned int channels, sampleRate;

			drflac_uint64 frameCount;

			drflac_int16* decoded = drflac_open_file_and_read_pcm_frames_s16(sourcePath.string().c_str(), &channels, &sampleRate, &frameCount, nullptr);

			if (!decoded)
			{
				Core::Log::Error("AudioCooker: Failed to decode FLAC: " + sourcePath.string());
				return {};
			}

			outSampleRate = sampleRate;
			outFrameCount = static_cast<uint32_t>(frameCount);
			outChannels = static_cast<uint8_t>(channels);

			wavPath = tempDir / (uuidString + "_temp.wav");
			tempWav = true;

			drwav_data_format format{};
			format.container = drwav_container_riff;
			format.format = DR_WAVE_FORMAT_PCM;
			format.channels = channels;
			format.sampleRate = sampleRate;
			format.bitsPerSample = 16;

			drwav wav;
			drwav_init_file_write(&wav, wavPath.string().c_str(), &format, nullptr);
			drwav_write_pcm_frames(&wav, frameCount, decoded);
			drwav_uninit(&wav);

			drflac_free(decoded, nullptr);

			Core::Log::Info("AudioCooker: Converted FLAC to temp WAV: " + wavPath.string());
		}
		else
		{
			Core::Log::Error("AudioCooker: Unsupported format for DSP-ADPCM cooking: " + extension);
			return {};
		}

		std::string vgaudio = GetVGAudioCliPath();
		std::vector<std::vector<uint8_t>> channelBlobs;

		for (uint8_t channel = 0; channel < outChannels; channel++)
		{
			std::filesystem::path dspPath = tempDir / (uuidString + "_ch" + std::to_string(channel) + ".dsp");

			std::string command = vgaudio +
				" -i:" + std::to_string(channel) +
				" " + wavPath.string() +
				" -o " + dspPath.string();

			Core::Log::Info("AudioCooker: " + command);
			int result_code = std::system(command.c_str());
			if (result_code != 0)
			{
				Core::Log::Error("AudioCooker: VGAudioCli failed for channel " + std::to_string(channel));
				if (tempWav)
					std::filesystem::remove(wavPath);
				return {};
			}

			std::vector<uint8_t> dspData = ReadFile(dspPath);
			// DSP data smaller than header
			if (dspData.size() < 96)
			{
				Core::Log::Error("AudioCooker: DSP file too small for channel " + std::to_string(channel));
				std::filesystem::remove(dspPath);
				if (tempWav)
					std::filesystem::remove(wavPath);
				return {};
			}

			channelBlobs.push_back(std::move(dspData));
			std::filesystem::remove(dspPath);
		}

		if (tempWav)
			std::filesystem::remove(wavPath);

		std::vector<uint8_t> result;

		// Write blob sizes
		for (const auto& blob : channelBlobs)
		{
			uint32_t size = static_cast<uint32_t>(blob.size());
			result.insert(result.end(), reinterpret_cast<uint8_t*>(&size), reinterpret_cast<uint8_t*>(&size) + 4);
		}

		// Write blobs
		for (const auto& blob : channelBlobs)
			result.insert(result.end(), blob.begin(), blob.end());

		return result;
	}
}
