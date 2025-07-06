#pragma once

#include <vector>

#ifdef _WIN32
#define DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT extern "C"
#endif

namespace Nightbird
{
	class SceneObject;
}

using CreateCustomObject = Nightbird::SceneObject* (*)(const char* name);

struct CustomObjectDescriptor
{
	const char* name;
	CreateCustomObject create;
};

inline std::vector<CustomObjectDescriptor>& GetCustomObjectRegistry()
{
	static std::vector<CustomObjectDescriptor> registry;
	return registry;
}

template <typename T>
class CustomObjectRegistrar
{
public:
	CustomObjectRegistrar(const char* registeredName)
	{
		GetCustomObjectRegistry().push_back({registeredName, [](const char* instanceName) -> Nightbird::SceneObject* { return new T(instanceName); }});
	}
};

#define REGISTER_SCENE_OBJECT(ClassName) \
	static CustomObjectRegistrar<ClassName> _registrar_##ClassName(#ClassName);

#define REGISTER_CUSTOM_OBJECT() \
	bool IsCustomObject() const override { return true; }

DLL_EXPORT int GetCustomObjectCount();
DLL_EXPORT const CustomObjectDescriptor* GetCustomObjectDescriptor(int index);
DLL_EXPORT void DeleteCustomObject(Nightbird::SceneObject* object);