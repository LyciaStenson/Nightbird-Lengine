#include "Glfw/AudioProvider.h"

namespace Nightbird::Glfw
{
	void AudioProvider::Initialize()
	{

	}

	void AudioProvider::Shutdown()
	{

	}

	void AudioProvider::Update()
	{

	}

	Audio::Handle AudioProvider::Play(const Core::AudioAsset& audio, bool loop)
	{
		return Audio::InvalidHandle;
	}

	void AudioProvider::PlayOnce(const Core::AudioAsset& audio)
	{

	}

	void AudioProvider::Stop(Audio::Handle handle)
	{

	}

	void AudioProvider::Pause(Audio::Handle handle)
	{

	}

	void AudioProvider::Resume(Audio::Handle handle)
	{

	}

	void AudioProvider::SetVolume(Audio::Handle handle, float volume)
	{

	}

	bool AudioProvider::IsPlaying(Audio::Handle handle) const
	{
		return false;
	}
}
