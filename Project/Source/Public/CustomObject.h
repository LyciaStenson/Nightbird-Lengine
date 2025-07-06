#pragma once

#include <string>

#include <Core/SceneObject.h>

#include <Core/ProjectExport.h>

class CustomObject : public Nightbird::SceneObject
{
public:
	CustomObject();
	CustomObject(const std::string& name);

	int testVar = 5;

	REGISTER_CUSTOM_OBJECT()
};