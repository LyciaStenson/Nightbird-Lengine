#pragma once

#ifdef _WIN32
	#ifdef PROJECT_BUILD
		#define PROJECT_API extern "C" __declspec(dllexport)
	#else
		#define PROJECT_API extern "C"
	#endif
#else
	#define DLL_EXPORT extern "C"
#endif

struct CustomObjectDescriptor;

PROJECT_API int GetCustomObjectCount();
PROJECT_API const CustomObjectDescriptor* GetCustomObjectDescriptor(int index);
PROJECT_API void DeleteCustomObject(void* object);

PROJECT_API void RegisterProjectTypes();