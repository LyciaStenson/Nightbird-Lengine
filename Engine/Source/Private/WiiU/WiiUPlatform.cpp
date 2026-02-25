#include "WiiUPlatform.h"

#include <wgb/proc.h>

namespace Nightbird
{
	void WiiUPlatform::Initialize()
	{
		WHBProcInit();
	}

	void WiiUPlatform::Shutdown()
	{
		WHNProcShutdown();
	}

	void WiiUPlatform::PollEvents()
	{

	}

	void WiiUPlatform::WaitEvents()
	{

	}

	bool WiiUPlatform::ShouldClose() const
	{
		return !WHBProcIsRunning();
	}

	void WiiUPlatform::GetFramebufferSize(int* width, int* height) const
	{
		*width = 1280;
		*height = 720;
	}
}