#include <Core/ProjectExport.h>

#include <CustomObject.h>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>

PROJECT_API int GetCustomObjectCount()
{
	return static_cast<int>(GetCustomObjectRegistry().size());
}

PROJECT_API const CustomObjectDescriptor* GetCustomObjectDescriptor(int index)
{
	const auto& registry = GetCustomObjectRegistry();
	return (index >= 0 && index < static_cast<int>(registry.size())) ? &registry[index] : nullptr;
}

PROJECT_API void DeleteCustomObject(void* object)
{
	delete object;
}

PROJECT_API void Test(void* object)
{
	delete object;
}

PROJECT_API void RegisterProjectTypes()
{
	std::cout << "Register Project Types" << std::endl;
}