#include "Core/AudioSource.h"

#include "Audio/AudioProvider.h"
#include "Core/AudioAsset.h"
#include "Core/Engine.h"
#include "Core/Log.h"

namespace Nightbird::Core
{
	NB_OBJECT_IMPL(AudioSource, SceneObject)

	void AudioSource::EnterScene()
	{
		if (m_PlayOnStart && m_AudioAsset)
			Play();
	}

	void AudioSource::Tick(float delta)
	{

	}

	void AudioSource::Play()
	{
		Engine* engine = GetEngine();
		if (!engine)
		{
			Core::Log::Warning("AudioSource: Play called before added to scene");
			return;
		}

		if (!m_AudioAsset)
		{
			Core::Log::Warning("AudioSource: Missing audio asset");
			return;
		}

		if (m_Handle != Audio::InvalidHandle)
			Stop();

		m_Handle = engine->GetAudioProvider().Play(*m_AudioAsset, m_Loop);

		if (m_Handle == Audio::InvalidHandle)
		{
			Core::Log::Warning("AudioSource: Failed to play audio");
			return;
		}

		engine->GetAudioProvider().SetVolume(m_Handle, m_Volume);
	}

	void AudioSource::Stop()
	{
		if (m_Handle == Audio::InvalidHandle)
			return;

		Engine* engine = GetEngine();
		if (!engine)
			return;

		engine->GetAudioProvider().Stop(m_Handle);
		m_Handle = Audio::InvalidHandle;
	}

	void AudioSource::Pause()
	{
		if (m_Handle == Audio::InvalidHandle)
			return;

		Engine* engine = GetEngine();
		if (!engine)
			return;

		engine->GetAudioProvider().Pause(m_Handle);
	}

	void AudioSource::Resume()
	{
		if (m_Handle == Audio::InvalidHandle)
			return;

		Engine* engine = GetEngine();
		if (!engine)
			return;

		engine->GetAudioProvider().Resume(m_Handle);
	}

	void AudioSource::SetVolume(float volume)
	{
		if (m_Handle == Audio::InvalidHandle)
			return;

		Engine* engine = GetEngine();
		if (!engine)
			return;

		engine->GetAudioProvider().SetVolume(m_Handle, volume);
	}

	bool AudioSource::IsPlaying() const
	{
		if (m_Handle == Audio::InvalidHandle)
			return false;

		Engine* engine = GetEngine();
		if (!engine)
			return false;

		return engine->GetAudioProvider().IsPlaying(m_Handle);
	}

	const std::shared_ptr<AudioAsset>& AudioSource::GetAudioAsset() const
	{
		return m_AudioAsset;
	}

	bool AudioSource::GetLoop() const
	{
		return m_Loop;
	}

	bool AudioSource::GetPlayOnStart() const
	{
		return m_PlayOnStart;
	}

	void AudioSource::SetAudioAsset(std::shared_ptr<AudioAsset> asset)
	{
		m_AudioAsset = std::move(asset);
	}

	void AudioSource::SetLoop(bool loop)
	{
		m_Loop = loop;
	}

	void AudioSource::SetPlayOnStart(bool playOnStart)
	{
		m_PlayOnStart = playOnStart;
	}
}
