#include <Player.h>

#include <iostream>

#include <GetInputSystem.h>

Player::Player(const std::string& name)
	: SceneObject(name)
{
	GetInputSystem()->BindKey("Jump", GLFW_KEY_SPACE);
	
	//GetInputSystem()->SubscribeActionPressed("Jump", [](const std::string& action)
	//{
		//std::cout << action << " pressed" << std::endl;
	//});
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