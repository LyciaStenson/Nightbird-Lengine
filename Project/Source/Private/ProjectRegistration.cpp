#include <Core/ProjectRegistration.h>

std::vector<CustomObjectDescriptor>& GetCustomObjectRegistry()
{
	static std::vector<CustomObjectDescriptor> registry;
	return registry;
}