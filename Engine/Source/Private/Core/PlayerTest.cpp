#include <Core/PlayerTest.h>

#include <iostream>

#include <Input.h>

using namespace Nightbird;

PlayerTest::PlayerTest(const std::string& name)
	: SceneObject(name)
{
	Input::Get().BindKey("Jump", GLFW_KEY_SPACE);

	Input::Get().SubscribeActionPressed("Jump", [](const char* action)
		{
			std::cout << action << " pressed" << std::endl;
		}
	);
}

RTTR_REGISTRATION
{
	rttr::registration::class_<PlayerTest>("PlayerTest")
	.constructor<std::string>();

	rttr::registration::method("CreatePlayerTest", [](const std::string& name) -> Nightbird::SceneObject*
	{
		return new PlayerTest(name);
	});
}