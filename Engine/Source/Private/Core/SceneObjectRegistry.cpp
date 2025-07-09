#include <Core/SceneObjectRegistry.h>

std::vector<SceneObjectDescriptor>& GetSceneObjectRegistry()
{
	static std::vector<SceneObjectDescriptor> registry;
	return registry;
}