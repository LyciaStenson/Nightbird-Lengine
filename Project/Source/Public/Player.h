#pragma once

#include <Core/SceneObject.h>

class Player : public Nightbird::SceneObject
{
public:
	using Nightbird::SceneObject::SceneObject;
	~Player() override = default;

	void EnterScene() override;
	void Tick(float delta) override;
	
	void OnJump();

protected:
	float movementSpeed = 5.0f;

	RTTR_ENABLE(Nightbird::SceneObject)
	RTTR_REGISTRATION_FRIEND
};