#include <Core/SceneObject.h>

using namespace Nightbird;

SceneObject::SceneObject(const std::string& name)
	: name(name)
{
	this->transform.owner = this;
}

const std::string& SceneObject::GetName() const
{
	return name;
}