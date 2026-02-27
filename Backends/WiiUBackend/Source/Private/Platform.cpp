#include "Platform.h"

#include <whb/proc.h>

#include <whb/log_cafe.h>

namespace Nightbird::WiiU
{
	void Platform::Initialize()
	{
		WHBProcInit();
		WHBLogCafeInit();
	}

	void Platform::Shutdown()
	{
		WHBLogCafeDeinit();
		WHBProcShutdown();
	}

	void Platform::PollEvents()
	{

	}

	void Platform::WaitEvents()
	{

	}

	bool Platform::ShouldClose() const
	{
		return !WHBProcIsRunning();
	}

	void Platform::GetFramebufferSize(int* width, int* height) const
	{
		*width = 1280;
		*height = 720;
	}

	Input::Provider& Platform::GetInputProvider()
	{
		return m_InputProvider;
	}
}