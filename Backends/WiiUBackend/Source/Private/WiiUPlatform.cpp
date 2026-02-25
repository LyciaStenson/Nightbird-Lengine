#include "WiiUPlatform.h"

#include <whb/proc.h>

namespace Nightbird
{
	void WiiUPlatform::Initialize()
	{
		WHBProcInit();
	}

	void WiiUPlatform::Shutdown()
	{
		WHBProcShutdown();
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