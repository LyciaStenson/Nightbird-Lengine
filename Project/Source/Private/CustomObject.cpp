#include <CustomObject.h>

#include <iostream>

#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

CustomObject::CustomObject()
{
	std::cout << "Creating custom object!" << std::endl;
}

CustomObject::CustomObject(const std::string& name)
	: SceneObject(name)
{
	std::cout << "Creating custom object!" << std::endl;
}

void RegisterType()
{

}

REGISTER_SCENE_OBJECT(CustomObject)