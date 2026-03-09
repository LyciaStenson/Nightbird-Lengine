#include "Platform.h"

#include "Core/Log.h"

#include <whb/proc.h>
#include <whb/log.h>
#include <whb/log_cafe.h>

namespace Nightbird::WiiU
{
	void Platform::Initialize()
	{
		WHBProcInit();
		WHBLogCafeInit();

		Core::Log::SetInfoFunc([](const std::string& message) { WHBLogPrintf("[Info] %s", message.c_str()); });
		Core::Log::SetWarningFunc([](const std::string& message) { WHBLogPrintf("[Warning] %s", message.c_str()); });
		Core::Log::SetErrorFunc([](const std::string& message) { WHBLogPrintf("[Error] %s", message.c_str()); });
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
