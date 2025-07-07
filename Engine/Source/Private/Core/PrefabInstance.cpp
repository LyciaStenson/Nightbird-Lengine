#include <Core/PrefabInstance.h>

namespace Nightbird
{
	PrefabInstance::PrefabInstance(const std::string& name, const std::string& prefabPath)
		: SceneObject(name), prefabPath(prefabPath)
	{

	}

	PrefabInstance::~PrefabInstance()
	{

	}

	const std::string& PrefabInstance::GetPrefabPath() const
	{
		return prefabPath;
	}
}