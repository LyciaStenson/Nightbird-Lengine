#include "EditorCamera.h"

#include <iostream>

#include <imgui.h>

#include <Input.h>

namespace Nightbird
{
	void EditorCamera::Tick(float delta)
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
		
		float yawDelta = deltaX * lookSensitivity;
		float pitchDelta = deltaY * lookSensitivity;
		
		glm::quat pitchQuat = glm::angleAxis(pitchDelta, right);
		glm::quat yawQuat = glm::angleAxis(yawDelta, glm::vec3(0.0f, 1.0f, 0.0f));

		transform.rotation = glm::normalize(yawQuat * pitchQuat * transform.rotation);
	}

	void EditorCamera::SetLastMousePos(double x, double y)
	{
		lastX = x;
		lastY = y;
	}
}