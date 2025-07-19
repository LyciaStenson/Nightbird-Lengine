#include <Player.h>

#include <iostream>

#include <Input.h>

using namespace Nightbird;

Player::Player(const std::string& name)
	: SceneObject(name)
{
	Input::Get().BindKey("Jump", GLFW_KEY_SPACE);
	
	Input::Get().SubscribeActionPressed("Jump", [](const std::string& action)
	{
		std::cout << action << " pressed" << std::endl;
	});
}

RTTR_PLUGIN_REGISTRATION
{
	rttr::registration::class_<Player>("Player")
	.constructor<std::string>();

	rttr::registration::method("CreatePlayer", [](const std::string& name) -> Nightbird::SceneObject*
	{
		return new Player(name);
	});
}