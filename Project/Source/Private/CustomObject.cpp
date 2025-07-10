#include <CustomObject.h>

#include <iostream>

CustomObject::CustomObject()
{
	std::cout << "Creating custom object!" << std::endl;
}

CustomObject::CustomObject(const std::string& name)
	: SceneObject(name)
{
	std::cout << "Creating custom object!" << std::endl;
}

void CustomObject::Serialize(json& out) const
{
	SceneObject::Serialize(out);
	out["testVar"] = testVar;
}

void CustomObject::Deserialize(const json& in)
{
	SceneObject::Deserialize(in);
	testVar = in.at("testVar").get<decltype(testVar)>();
}

RTTR_REGISTRATION
{
	rttr::registration::class_<CustomObject>("CustomObject")
	.constructor<>()
	.property("testVar", &CustomObject::testVar);
}

REGISTER_SCENE_OBJECT(CustomObject)