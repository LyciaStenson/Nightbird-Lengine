#pragma once

#include "Core/SceneObject.h"

#include "Audio/AudioHandle.h"

#include <uuid.h>

#include <memory>

namespace Nightbird::Core
{
	class AudioAsset;

	class AudioSource : public SceneObject
	{
	public:
		NB_OBJECT()
		NB_ENABLE(SceneObject)
		NB_REGISTRATION_FRIEND

		using SceneObject::SceneObject;

		void LoadAssets(AssetLoader& assetLoader) override;
		void EnterScene() override;
		void Tick(float delta) override;

		void Play();
		void Stop();
		void Pause();
		void Resume();

		bool IsPlaying() const;

		float GetVolume() const;
		void SetVolume(float volume);

		const std::shared_ptr<AudioAsset>& GetAudioAsset() const;
		void SetAudioAsset(std::shared_ptr<AudioAsset> asset);

		bool GetLoop() const;
		void SetLoop(bool loop);

		bool GetPlayOnStart() const;
		void SetPlayOnStart(bool playOnStart);

		const uuids::uuid& GetAudioUUID() const;
		void SetAudioUUID(const uuids::uuid& uuid);

	private:
		uuids::uuid m_AudioUUID;
		std::shared_ptr<AudioAsset> m_AudioAsset;
		Audio::Handle m_Handle = Audio::InvalidHandle;
		bool m_Loop = false;
		bool m_PlayOnStart = true;
		float m_Volume = 1.0f;
	};
}
