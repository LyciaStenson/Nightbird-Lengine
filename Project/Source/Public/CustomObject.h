#pragma once

#include <string>

#include <Core/SceneObject.h>

#include <Core/ProjectRegistration.h>

class CustomObject : public Nightbird::SceneObject
{
public:
	CustomObject();
	CustomObject(const std::string& name);

	int testVar = 5;

	//template <class Archive>
	//void serialize(Archive& archive)
	//{
	//	archive
	//	(
	//		cereal::base_class<::Nightbird::SceneObject>(this),
	//		CEREAL_NVP(testVar)
	//	);
	//}
	
	//SERIALIZE_FIELDS(testVar)
	REGISTER_IS_CUSTOM_OBJECT()
};

extern "C" __declspec(dllexport) void RegisterType();