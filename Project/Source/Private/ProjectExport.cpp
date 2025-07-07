#include <Core/ProjectExport.h>

#include <CustomObject.h>

int GetCustomObjectCount()
{
	return static_cast<int>(GetCustomObjectRegistry().size());
}

const CustomObjectDescriptor* GetCustomObjectDescriptor(int index)
{
	const auto& registry = GetCustomObjectRegistry();
	return (index >= 0 && index < static_cast<int>(registry.size())) ? &registry[index] : nullptr;
}

void DeleteCustomObject(void* object)
{
	delete object;
}