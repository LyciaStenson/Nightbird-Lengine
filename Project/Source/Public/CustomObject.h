#pragma once

#include <string>

#include <Core/SceneObject.h>

#include <Core/SceneObjectRegistry.h>

class CustomObject : public Nightbird::SceneObject
{
public:
	CustomObject(const std::string& name);

	void Serialize(json& out) const override;
	void Deserialize(const json& in) override;

	int testVar = 5;
	
	REGISTER_IS_CUSTOM_OBJECT()

	RTTR_ENABLE(Nightbird::SceneObject)
};