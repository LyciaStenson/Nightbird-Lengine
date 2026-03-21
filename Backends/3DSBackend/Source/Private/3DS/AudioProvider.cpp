#include "3DS/AudioProvider.h"

#include "Load/AudioLoader.h"

#include "Core/Log.h"

#include <uuid.h>

#include <cstring>

namespace Nightbird::N3DS
{
	void AudioProvider::Initialize()
	{
		Result rc = ndspInit();
		if (R_FAILED(rc))
		{
			Core::Log::Error("Failed to initialize NDSP. Check dspfirm.cdc is present.");
			return;
		}

		ndspSetOutputMode(NDSP_OUTPUT_STEREO);
		ndspChnSetInterp(k_Channel, NDSP_INTERP_LINEAR);

		Load::AudioLoader loader;

		auto uuid = uuids::uuid::from_string("2322e33c-17db-45d1-94e4-846986d0079c");
		if (!uuid)
		{
			Core::Log::Error("AudioProvider: Invalid UUID");
			ndspExit();
			return;
		}

		auto audio = loader.Load("sdmc:/Nightbird/Cooked", *uuid);
		if (!audio)
		{
			Core::Log::Error("AudioProvider: Failed to load audio asset");
			ndspExit();
			return;
		}

		if (!m_AudioBuffer.Load(*audio))
		{
			Core::Log::Error("AudioProvider: Failed to initialize N3DS::Audio");
			ndspExit();
			return;
		}

		m_AudioBuffer.Play(k_Channel);

		m_Initialized = true;
		Core::Log::Info("Audio provider initialized");
	}

	void AudioProvider::Shutdown()
	{
		if (!m_Initialized)
			return;

		m_AudioBuffer.Stop(k_Channel);
		ndspExit();
		m_Initialized = false;
	}

	void AudioProvider::Update()
	{

	}
}
