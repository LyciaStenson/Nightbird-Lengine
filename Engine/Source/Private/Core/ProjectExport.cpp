#include <Core/ProjectExport.h>

#include <Core/SceneObject.h>

DLL_EXPORT int GetCustomObjectCount()
{
	return static_cast<int>(GetCustomObjectRegistry().size());
}

DLL_EXPORT const CustomObjectDescriptor* GetCustomObjectDescriptor(int index)
{
	const auto& registry = GetCustomObjectRegistry();
	return (index >= 0 && index < static_cast<int>(registry.size())) ? &registry[index] : nullptr;
}

DLL_EXPORT void DeleteCustomObject(Nightbird::SceneObject* object)
{
	delete object;
}