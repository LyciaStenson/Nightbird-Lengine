#pragma once

#include <Core/SceneObject.h>

class PlayerTest : public Nightbird::SceneObject
{
public:
	PlayerTest(const std::string& name);
	~PlayerTest() override = default;
	
	RTTR_ENABLE(Nightbird::SceneObject)
};