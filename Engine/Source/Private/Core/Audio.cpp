#include "Core/Audio.h"

namespace Nightbird::Core
{
	Audio::Audio(uint32_t sampleRate, uint32_t frameCount, uint8_t channels, AudioEncoding encoding, std::vector<std::vector<uint8_t>> channelData)
		: m_SampleRate(sampleRate), m_FrameCount(frameCount), m_Channels(channels), m_Encoding(encoding), m_ChannelData(std::move(channelData))
	{

	}

	uint32_t Audio::GetSampleRate() const
	{
		return m_SampleRate;
	}

	uint32_t Audio::GetFrameCount() const
	{
		return m_FrameCount;
	}

	uint8_t Audio::GetChannels() const
	{
		return m_Channels;
	}

	AudioEncoding Audio::GetEncoding() const
	{
		return m_Encoding;
	}

	const std::vector<uint8_t>& Audio::GetChannelData(uint8_t channel) const
	{
		return m_ChannelData[channel];
	}
}
