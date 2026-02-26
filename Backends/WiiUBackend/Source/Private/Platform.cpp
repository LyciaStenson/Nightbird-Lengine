#include "Platform.h"

#include <coreinit/systeminfo.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <nn/ac.h>

#include <whb/log.h>
#include <whb/log_console.h>
#include <whb/proc.h>

#include <thread>

//int hello_thread()
//{
//	int last_tm_sec = -1;
//	uint32_t ip = 0;
//	WHBLogPrintf("Nightbird Engine");
//
//	while (WHBProcIsRunning())
//	{
//		OSCalendarTime tm;
//		OSTicksToCalendarTime(OSGetTime(), &tm);
//
//		if (tm.tm_sec != last_tm_sec)
//		{
//			WHBLogPrintf("%02d/%02d/%04d %02d:%02d:%02d I'm still here.",
//				tm.tm_mday, tm.tm_mon, tm.tm_year,
//				tm.tm_hour, tm.tm_min, tm.tm_sec);
//			last_tm_sec = tm.tm_sec;
//		}
//
//		WHBLogConsoleDraw();
//		OSSleepTicks(OSMillisecondsToTicks(100));
//	}
//
//	WHBLogPrintf("Exiting... good bye.");
//	WHBLogConsoleDraw();
//	OSSleepTicks(OSMillisecondsToTicks(1000));
//	return 0;
//}

namespace Nightbird::WiiU
{
	void Platform::Initialize()
	{
		//nn::ac::ConfigIdNum configId;

		//nn::ac::Initialize();
		//nn::ac::GetStartupId(&configId);
		//nn::ac::Connect(configId);

		WHBProcInit();
		WHBLogConsoleInit();

		WHBLogPrintf("Nightbird Engine");

		//std::thread t(hello_thread);
		//t.join();
	}

	void Platform::Shutdown()
	{
		WHBLogConsoleFree();
		WHBProcShutdown();

		nn::ac::Finalize();
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