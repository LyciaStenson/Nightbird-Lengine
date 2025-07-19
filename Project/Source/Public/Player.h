#pragma once

#include <Core/SceneObject.h>

class Player : public Nightbird::SceneObject
{
public:
	Player(const std::string& name);
	~Player() override = default;

	RTTR_ENABLE(Nightbird::SceneObject)
};