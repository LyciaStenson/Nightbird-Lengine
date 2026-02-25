#pragma once

#include "Core/Platform.h"

namespace Nightbird
{
	class WiiUPlatform : public Platform
	{
	public:
		virtual void Initialize() override;
		virtual void Shutdown() override;
		virtual void PollEvents() override;
		virtual void WaitEvents() override;
		virtual bool ShouldClose() const override;
		virtual void GetFramebufferSize(int* width, int* height) const override;
	};
}