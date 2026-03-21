#pragma once

#include "Audio/AudioProvider.h"
#include "Audio/AudioHandle.h"

#include "Core/AudioAsset.h"

namespace Nightbird::Glfw
{
	class AudioProvider : public Audio::Provider
	{
	public:
		void Initialize() override;
		void Shutdown() override;
		void Update() override;

		Audio::Handle Play(const Core::AudioAsset& audio, bool loop);
		void PlayOnce(const Core::AudioAsset& audio);
		void Stop(Audio::Handle handle);
		void Pause(Audio::Handle handle);
		void Resume(Audio::Handle handle);

		void SetVolume(Audio::Handle handle, float volume);

		bool IsPlaying(Audio::Handle handle) const;
	};
}
