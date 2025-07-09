#pragma once

#include <string>
#include <vector>

namespace Nightbird
{
	class SceneObject;
}

using CreateSceneObject = Nightbird::SceneObject* (*)(const char* name);

struct SceneObjectDescriptor
{
	const char* typeName;
	CreateSceneObject create;
};

std::vector<SceneObjectDescriptor>& GetSceneObjectRegistry();

template <typename T>
class SceneObjectRegistrar
{
public:
	SceneObjectRegistrar(const char* name)
	{
		GetSceneObjectRegistry().push_back
		(
			{
				name,
				[](const char* instanceName) -> Nightbird::SceneObject*
			{
				return new T(instanceName);
			}
			}
		);
	}
};

#define REGISTER_SCENE_OBJECT(Type) \
	static SceneObjectRegistrar<Type> _registrar_##Type(#Type);

#define REGISTER_IS_CUSTOM_OBJECT() \
	bool IsCustomObject() const override { return true; }