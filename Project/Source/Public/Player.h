#pragma once

#include <Core/SpatialObject.h>

class Player : public Nightbird::SpatialObject
{
public:
	using Nightbird::SpatialObject::SpatialObject;
	~Player() override = default;

	void EnterScene() override;
	void Tick(float delta) override;
	
	void OnJump();

protected:
	float movementSpeed = 5.0f;

	RTTR_ENABLE(Nightbird::SpatialObject)
	RTTR_REGISTRATION_FRIEND
};