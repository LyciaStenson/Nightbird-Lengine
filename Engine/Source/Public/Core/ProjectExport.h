#pragma once

#ifdef _WIN32
	#ifdef PROJECT_BUILD
		#define NB_EXPORT extern "C" __declspec(dllexport)
	#else
		#define NB_EXPORT extern "C"// __declspec(dllimport)
	#endif
#else
	#define DLL_EXPORT extern "C"
#endif

struct SceneObjectDescriptor;

NB_EXPORT int GetSceneObjectCount();
NB_EXPORT const SceneObjectDescriptor* GetSceneObjectDescriptor(int index);
NB_EXPORT void DeleteCustomObject(void* object);