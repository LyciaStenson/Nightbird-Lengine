#pragma once

#include <GLFW/glfw3.h>

namespace Nightbird
{
	class Renderer;

	struct WindowUserData
	{
		Renderer* renderer;
	};

	class GlfwWindow
	{
	public:
		GlfwWindow();
		~GlfwWindow();

		GLFWwindow* Get() const;

		void SetUserPointer(Renderer* renderer);

		void GetFramebufferSize(int* width, int* height);

	private:
		GLFWwindow* m_Window = nullptr;

		WindowUserData m_UserData = {};

		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	};
}