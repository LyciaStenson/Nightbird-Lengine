#pragma once

#include "Core/SceneObject.h"

#include "Audio/AudioHandle.h"

#include <memory>

namespace Nightbird::Core
{
	class AudioAsset;

	class AudioSource : public SceneObject
	{
	public:
		NB_OBJECT(AudioSource, SceneObject)

		using SceneObject::SceneObject;

		void EnterScene() override;
		void Tick(float delta) override;

		void Play();
		void Stop();
		void Pause();
		void Resume();

		void SetVolume(float volume);

		bool IsPlaying() const;

		const std::shared_ptr<AudioAsset>& GetAudioAsset() const;
		bool GetLoop() const;
		bool GetPlayOnStart() const;

		void SetAudioAsset(std::shared_ptr<AudioAsset> asset);
		void SetLoop(bool loop);
		void SetPlayOnStart(bool playOnStart);

	private:
		std::shared_ptr<AudioAsset> m_AudioAsset;
		Audio::Handle m_Handle = Audio::InvalidHandle;
		bool m_Loop = false;
		bool m_PlayOnStart = true;
		float m_Volume = 1.0f;
	};
}
