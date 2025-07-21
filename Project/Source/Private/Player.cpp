#include <Player.h>

#include <iostream>

#include <Input.h>

using namespace Nightbird;

void Player::EnterScene()
{
	Input::Get().BindKey("Jump", GLFW_KEY_SPACE);
	Input::Get().SubscribeActionPressed("Jump", [this]() { this->OnJump(); });
}

void Player::Tick(float delta)
{
	if (Input::Get().IsKeyPressed(GLFW_KEY_W))
		transform.position.x += delta * movementSpeed;
}

void Player::OnJump()
{
	std::cout << "Jump" << std::endl;
}

RTTR_PLUGIN_REGISTRATION
{
	rttr::registration::class_<Player>("Player")
	.constructor<std::string>()
	.property("MovementSpeed", &Player::movementSpeed);

	rttr::registration::method("CreatePlayer", [](const std::string& name) -> Nightbird::SceneObject*
	{
		return new Player(name);
	});
}