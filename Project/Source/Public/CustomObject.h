#pragma once

#include <Core/SceneObject.h>

class CustomObject : public Nightbird::SceneObject
{
public:
	CustomObject(const std::string& name);
	~CustomObject() = default;
	
	int testVar = 5;

	RTTR_ENABLE(Nightbird::SceneObject)
};