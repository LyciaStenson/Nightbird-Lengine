#pragma once

#include "Audio/AudioProvider.h"

#include "3DS/AudioBuffer.h"

#include <3ds.h>

#include <cstddef>

namespace Nightbird::N3DS
{
	class AudioProvider : public Audio::Provider
	{
	public:
		void Initialize() override;
		void Shutdown() override;
		void Update() override;

	private:
		static constexpr int k_Channel = 0;
		AudioBuffer m_AudioBuffer;
		bool m_Initialized = false;
	};
}
