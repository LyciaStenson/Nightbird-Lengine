#include <EditorCamera.h>

#include <iostream>

#include <imgui.h>

#include <Input.h>

namespace Nightbird
{
	void EditorCamera::Tick(float delta)
	{
		static bool looking = false;
		static bool first = true;

		if (!ImGui::IsWindowHovered() || !ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			looking = false;
			first = true;
			return;
		}

		looking = true;
		
		auto& input = Input::Get();
		
		glm::vec3 movementDir(0.0f);

		if (Input::Get().IsKeyPressed(GLFW_KEY_W))
			transform.position.z -= delta * movementSpeed;
		if (Input::Get().IsKeyPressed(GLFW_KEY_S))
			transform.position.z += delta * movementSpeed;
		if (Input::Get().IsKeyPressed(GLFW_KEY_A))
			transform.position.x -= delta * movementSpeed;
		if (Input::Get().IsKeyPressed(GLFW_KEY_D))
			transform.position.x += delta * movementSpeed;
		if (Input::Get().IsKeyPressed(GLFW_KEY_Q))
			transform.position.y -= delta * movementSpeed;
		if (Input::Get().IsKeyPressed(GLFW_KEY_E))
			transform.position.y += delta * movementSpeed;
		
		double mouseX, mouseY;
		input.GetCursorPos(mouseX, mouseY);
		
		static double lastX = 0.0, lastY = 0.0;

		if (first)
		{
			lastX = mouseX;
			lastY = mouseY;
			first = false;
			return;
		}

		float deltaX = static_cast<float>(lastX - mouseX);
		float deltaY = static_cast<float>(lastY - mouseY);

		lastX = mouseX;
		lastY = mouseY;
		
		float yawDelta = deltaX * lookSensitivity;
		float pitchDelta = deltaY * lookSensitivity;

		glm::vec3 right = transform.rotation * glm::vec3(1.0f, 0.0f, 0.0f);
		glm::quat pitchQuat = glm::angleAxis(pitchDelta, right);

		glm::quat yawQuat = glm::angleAxis(yawDelta, glm::vec3(0.0f, 1.0f, 0.0f));

		transform.rotation = glm::normalize(yawQuat * pitchQuat * transform.rotation);
	}
}