#include "Core/AudioSource.h"

#include "Audio/AudioProvider.h"
#include "Core/AudioAsset.h"
#include "Core/Engine.h"
#include "Core/Log.h"

namespace Nightbird::Core
{
	NB_REFLECT(AudioSource, NB_PARENT(SceneObject), NB_FACTORY(AudioSource),
		NB_FIELD(m_AudioUUID, UUID)
	)

	void AudioSource::LoadAssets(AssetLoader& assetLoader)
	{
		m_AudioAsset = assetLoader.LoadAudio(m_AudioUUID);
	}

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

	bool AudioSource::IsPlaying() const
	{
		if (m_Handle == Audio::InvalidHandle)
			return false;

		Engine* engine = GetEngine();
		if (!engine)
			return false;

		return engine->GetAudioProvider().IsPlaying(m_Handle);
	}

	float AudioSource::GetVolume() const
	{
		return m_Volume;
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

	const std::shared_ptr<AudioAsset>& AudioSource::GetAudioAsset() const
	{
		return m_AudioAsset;
	}

	void AudioSource::SetAudioAsset(std::shared_ptr<AudioAsset> asset)
	{
		m_AudioAsset = std::move(asset);
	}

	bool AudioSource::GetLoop() const
	{
		return m_Loop;
	}

	void AudioSource::SetLoop(bool loop)
	{
		m_Loop = loop;
	}

	bool AudioSource::GetPlayOnStart() const
	{
		return m_PlayOnStart;
	}

	void AudioSource::SetPlayOnStart(bool playOnStart)
	{
		m_PlayOnStart = playOnStart;
	}

	const uuids::uuid& AudioSource::GetAudioUUID() const
	{
		return m_AudioUUID;
	}

	void AudioSource::SetAudioUUID(const uuids::uuid& uuid)
	{
		m_AudioUUID = uuid;
	}
}
