#include <Core/ProjectExport.h>

#include <Core/SceneObjectRegistry.h>

NB_EXPORT int GetSceneObjectCount()
{
	return static_cast<int>(GetSceneObjectRegistry().size());
}

NB_EXPORT const SceneObjectDescriptor* GetSceneObjectDescriptor(int index)
{
	const auto& registry = GetSceneObjectRegistry();
	if (index < 0 || index >= static_cast<int>(registry.size()))
		return nullptr;
	return &registry[index];
}

NB_EXPORT void DeleteCustomObject(void* object)
{
	delete object;
}