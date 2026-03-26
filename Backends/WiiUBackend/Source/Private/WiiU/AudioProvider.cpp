#include "WiiU/AudioProvider.h"

#include "Core/Log.h"

#include <coreinit/memdefaultheap.h>
#include <coreinit/cache.h>
#include <coreinit/thread.h>

#include <cstring>
#include <algorithm>

namespace Nightbird::WiiU
{
	void AudioProvider::Initialize()
	{
		uint32_t oldAffinity = OSGetThreadAffinity(OSGetCurrentThread());
		OSSetThreadAffinity(OSGetCurrentThread(), OS_THREAD_ATTRIB_AFFINITY_CPU1);

		AXInitParams params{};
		params.renderer = AX_INIT_RENDERER_48KHZ;
		params.pipeline = AX_INIT_PIPELINE_SINGLE;
		AXInitWithParams(&params);

		OSSetThreadAffinity(OSGetCurrentThread(), oldAffinity);

		m_Initialized = true;
		Core::Log::Info("AudioProvider: Initialized");
	}

	void AudioProvider::Shutdown()
	{
		if (!m_Initialized)
			return;

		for (auto& sound : m_ActiveSounds)
			DestroySound(*sound);
		m_ActiveSounds.clear();
		AXQuit();
		m_Initialized = false;
	}

	void AudioProvider::Update()
	{
		if (!m_Initialized)
			return;

		for (int i = static_cast<int>(m_ActiveSounds.size()) - 1; i >= 0; i--)
		{
			auto& sound = m_ActiveSounds[i];
			if (sound->playOnce)
			{
				if (!IsPlaying(sound->handle))
				{
					DestroySound(*sound);
					m_ActiveSounds.erase(m_ActiveSounds.begin() + i);
				}
			}
		}
	}

	Audio::Handle AudioProvider::Play(const Core::AudioAsset& audio, bool loop)
	{
		return StartSound(audio, loop, false);
	}

	void AudioProvider::PlayOnce(const Core::AudioAsset& audio)
	{
		StartSound(audio, false, true);
	}

	void AudioProvider::Stop(Audio::Handle handle)
	{
		if (!m_Initialized)
			return;

		ActiveSound* sound = FindSound(handle);
		if (!sound)
			return;

		DestroySound(*sound);

		m_ActiveSounds.erase(
			std::remove_if(m_ActiveSounds.begin(), m_ActiveSounds.end(),
				[handle](const auto& s)
				{
					return s->handle == handle;
				}),
			m_ActiveSounds.end());
	}

	void AudioProvider::Pause(Audio::Handle handle)
	{
		if (!m_Initialized)
			return;

		ActiveSound* sound = FindSound(handle);
		if (!sound)
			return;

		for (AXVoice* voice : sound->voices)
		{
			if (voice)
			{
				AXVoiceBegin(voice);
				AXSetVoiceState(voice, AX_VOICE_STATE_STOPPED);
				AXVoiceEnd(voice);
			}
		}
	}

	void AudioProvider::Resume(Audio::Handle handle)
	{
		if (!m_Initialized)
			return;

		ActiveSound* sound = FindSound(handle);
		if (!sound)
			return;

		for (AXVoice* voice : sound->voices)
		{
			if (voice)
			{
				AXVoiceBegin(voice);
				AXSetVoiceState(voice, AX_VOICE_STATE_PLAYING);
				AXVoiceEnd(voice);
			}
		}
	}

	void AudioProvider::SetVolume(Audio::Handle handle, float volume)
	{
		if (!m_Initialized)
			return;

		// ActiveSound* sound = FindSound(handle);
		// if (!sound)
		// 	return;
  //
		// uint16_t axVolume = static_cast<uint16_t>(volume * 0x8000);
  //
		// for (AXVoice* voice : sound->voices)
		// {
		// 	if (!voice)
		// 		continue;
  //
		// 	AXVoiceBegin(voice);
  //
		// 	AXVoiceVeData veData{};
		// 	veData.volume = axVolume;
		// 	veData.delta = 0;
		// 	AXSetVoiceVe(voice, &veData);
  //
		// 	AXVoiceDeviceMixData tvMix{};
		// 	tvMix.bus[0].volume = axVolume;
		// 	tvMix.bus[1].volume = axVolume;
		// 	AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_TV, 0, &tvMix);
  //
		// 	AXVoiceDeviceMixData drcMix{};
		// 	drcMix.bus[0].volume = axVolume;
		// 	drcMix.bus[1].volume = axVolume;
		// 	AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_DRC, 0, &drcMix);
  //
		// 	AXVoiceEnd(voice);
		// }
	}

	bool AudioProvider::IsPlaying(Audio::Handle handle) const
	{
		if (!m_Initialized)
			return false;

		const ActiveSound* sound = FindSound(handle);
		if (!sound)
			return false;

		for (AXVoice* voice : sound->voices)
			if (voice && AXIsVoiceRunning(voice))
				return true;

		return false;
	}

	Audio::Handle AudioProvider::StartSound(const Core::AudioAsset& audio, bool loop, bool playOnce)
	{
		if (!m_Initialized)
			return Audio::InvalidHandle;

		if (audio.GetEncoding() != Core::AudioEncoding::PCM16)
		{
			Core::Log::Error("AudioProvider: Only PCM16 supported");
			return Audio::InvalidHandle;
		}

		uint8_t channels = audio.GetChannels();
		if (channels == 0)
		{
			Core::Log::Error("AudioProvider: Audio has zero channels");
			return Audio::InvalidHandle;
		}

		uint32_t frames = audio.GetFrameCount();

		auto activeSound = std::make_unique<ActiveSound>();
		activeSound->handle = m_NextHandle++;
		activeSound->looping = loop;
		activeSound->playOnce = playOnce;
		activeSound->channels = channels;

		for (uint8_t channel = 0; channel < channels; ++channel)
		{
			const auto& channelData = audio.GetChannelData(channel);
			if (channelData.empty())
			{
				Core::Log::Error("AudioProvider: Channel " + std::to_string(channel) + " is empty");
				return Audio::InvalidHandle;
			}

			uint8_t* buffer = static_cast<uint8_t*>(MEMAllocFromDefaultHeapEx(channelData.size(), 64));
			if (!buffer)
			{
				Core::Log::Error("AudioProvider: Failed to allocate buffer for channel: " + std::to_string(channel));

				for (AXVoice* voice : activeSound->voices)
					AXFreeVoice(voice);
				for (auto* buffer : activeSound->buffers)
					MEMFreeToDefaultHeap(buffer);

				return Audio::InvalidHandle;
			}

			std::memcpy(buffer, channelData.data(), channelData.size());
			DCFlushRange(buffer, channelData.size());
			activeSound->buffers.push_back(buffer);

			AXVoice* voice = AXAcquireVoice(31, nullptr, nullptr);
			if (!voice)
			{
				Core::Log::Error("AudioProvider: Failed to acquire voice for channel " + std::to_string(channel));

				for (AXVoice* voice : activeSound->voices)
					AXFreeVoice(voice);
				for (auto* buffer : activeSound->buffers)
					MEMFreeToDefaultHeap(buffer);

				return Audio::InvalidHandle;
			}
			activeSound->voices.push_back(voice);

			SetupVoice(voice, buffer, frames, audio.GetSampleRate(), loop, channel, channels);

			AXVoiceBegin(voice);
			AXSetVoiceState(voice, AX_VOICE_STATE_PLAYING);
			AXVoiceEnd(voice);
		}

		if (activeSound->voices.empty())
		{
			Core::Log::Error("AudioProvider: Failed to create voices");
			return Audio::InvalidHandle;
		}

		Audio::Handle handle = activeSound->handle;
		m_ActiveSounds.push_back(std::move(activeSound));
		return handle;
	}

	void AudioProvider::SetupVoice(AXVoice* voice, void* data, uint32_t frames, uint32_t sampleRate, bool loop, uint8_t channel, uint8_t totalChannels)
	{
		AXVoiceBegin(voice);
		AXSetVoiceType(voice, AX_VOICE_TYPE_UNKNOWN);

		AXVoiceOffsets offsets{};
		offsets.dataType = AX_VOICE_FORMAT_LPCM16;
		offsets.loopingEnabled = loop ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED;
		offsets.loopOffset = 0;
		offsets.endOffset = frames - 1;
		offsets.currentOffset = 0;
		offsets.data = data;
		AXSetVoiceOffsets(voice, &offsets);

		AXSetVoiceSrcRatio(voice, static_cast<float>(sampleRate) / static_cast<float>(AXGetInputSamplesPerSec()));
		AXSetVoiceSrcType(voice, AX_VOICE_SRC_TYPE_LINEAR);

		AXVoiceVeData veData{};
		veData.volume = 0x8000;
		veData.delta = 0;
		AXSetVoiceVe(voice, &veData);

		AXVoiceDeviceMixData tvMix{};
		AXVoiceDeviceMixData drcMix{};

		if (totalChannels == 1)
		{
			// Mono: center
			tvMix.bus[0].volume = 0x8000;
			tvMix.bus[1].volume = 0x8000;
			drcMix.bus[0].volume = 0x8000;
			drcMix.bus[1].volume = 0x8000;
		}
		else if (totalChannels == 2)
		{
			if (channel == 0) // Left
			{
				tvMix.bus[0].volume = 0x8000;
				tvMix.bus[1].volume = 0x0000;
				drcMix.bus[0].volume = 0x8000;
				drcMix.bus[1].volume = 0x0000;
			}
			else if (channel == 1) // Right
			{
				tvMix.bus[0].volume = 0x0000;
				tvMix.bus[1].volume = 0x8000;
				drcMix.bus[0].volume = 0x0000;
				drcMix.bus[1].volume = 0x8000;
			}
		}
		else
		{
			// More than 2 channels: naive center
			tvMix.bus[0].volume = 0x8000;
			tvMix.bus[1].volume = 0x8000;
			drcMix.bus[0].volume = 0x8000;
			drcMix.bus[1].volume = 0x8000;
		}

		AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_TV, 0, &tvMix);
		AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_DRC, 0, &drcMix);

		AXVoiceEnd(voice);
	}

	AudioProvider::ActiveSound* AudioProvider::FindSound(Audio::Handle handle)
	{
		for (auto& sound : m_ActiveSounds)
		{
			if (sound->handle == handle)
				return sound.get();
		}
		return nullptr;
	}

	const AudioProvider::ActiveSound* AudioProvider::FindSound(Audio::Handle handle) const
	{
		for (auto& sound : m_ActiveSounds)
		{
			if (sound->handle == handle)
				return sound.get();
		}
		return nullptr;
	}

	void AudioProvider::DestroySound(ActiveSound& sound)
	{
		for (AXVoice* voice : sound.voices)
		{
			if (!voice)
				continue;

			AXVoiceBegin(voice);
			AXSetVoiceState(voice, AX_VOICE_STATE_STOPPED);
			AXVoiceEnd(voice);

			AXFreeVoice(voice);
		}

		for (auto* buffer : sound.buffers)
		{
			MEMFreeToDefaultHeap(buffer);
		}

		sound.voices.clear();
		sound.buffers.clear();
	}
}
