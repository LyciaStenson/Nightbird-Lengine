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
	float lookSensitivity = 5.0f;
	
	double lastX = 0.0, lastY = 0.0;
	bool firstMouse = true;
	
	RTTR_ENABLE(Nightbird::SpatialObject)
	RTTR_REGISTRATION_FRIEND
};