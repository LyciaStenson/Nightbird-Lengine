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
	auto& input = Input::Get();

	glm::vec3 forward = transform.rotation * glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 right = transform.rotation * glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 up = transform.rotation * glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 movementDir(0.0f);

	if (input.IsKeyPressed(GLFW_KEY_W))
		movementDir += forward;
	if (input.IsKeyPressed(GLFW_KEY_S))
		movementDir -= forward;
	if (input.IsKeyPressed(GLFW_KEY_A))
		movementDir -= right;
	if (input.IsKeyPressed(GLFW_KEY_D))
		movementDir += right;
	if (input.IsKeyPressed(GLFW_KEY_Q))
		movementDir -= up;
	if (input.IsKeyPressed(GLFW_KEY_E))
		movementDir += up;

	transform.position += movementDir * movementSpeed * delta;

	double mouseX, mouseY;
	input.GetCursorPos(mouseX, mouseY);

	float deltaX = static_cast<float>(lastX - mouseX);
	float deltaY = static_cast<float>(lastY - mouseY);

	lastX = mouseX;
	lastY = mouseY;

	float yawDelta = deltaX * lookSensitivity * 0.001f;
	float pitchDelta = deltaY * lookSensitivity * 0.001f;

	glm::quat pitchQuat = glm::angleAxis(pitchDelta, right);
	glm::quat yawQuat = glm::angleAxis(yawDelta, glm::vec3(0.0f, 1.0f, 0.0f));

	transform.rotation = glm::normalize(yawQuat * pitchQuat * transform.rotation);
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