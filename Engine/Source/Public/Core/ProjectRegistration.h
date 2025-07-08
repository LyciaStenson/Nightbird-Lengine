#pragma once

#include <vector>

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

std::vector<CustomObjectDescriptor>& GetCustomObjectRegistry();

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

#define REGISTER_IS_CUSTOM_OBJECT() \
	bool IsCustomObject() const override { return true; }

//#define REGISTER_SERIALIZATION(ClassName) \
//	CEREAL_REGISTER_TYPE(ClassName) \
//	CEREAL_REGISTER_POLYMORPHIC_RELATION(::Nightbird::SceneObject, ClassName)

//#define SERIALIZE_FIELDS(...) \
//	template <class Archive> \
//	void serialize(Archive& archive) \
//	{ \
//		archive \
//		( \
//			cereal::base_class<::Nightbird::SceneObject>(this), \
//			CEREAL_NVP(__VA_ARGS__) \
//		); \
//	}