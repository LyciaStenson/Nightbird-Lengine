#pragma once

//#include <Core/InputSystem.h>
//
//#ifdef _WIN32
//	#ifdef PROJECT_BUILD
//		#define PROJECT_API extern "C" __declspec(dllexport)
//	#else
//		#define PROJECT_API extern "C"
//	#endif
//#else
//	#define PROJECT_API extern "C"
//#endif
//
//namespace Nightbird
//{
//	struct EngineContext
//	{
//		InputSystem* inputSystem = nullptr;
//	};
//
//	PROJECT_API void InitProject(EngineContext* context);
//
//	InputSystem* GetInputSystem();
//}