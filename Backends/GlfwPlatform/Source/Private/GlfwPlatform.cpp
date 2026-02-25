#include "GlfwPlatform.h"

namespace Nightbird
{
	void GlfwPlatform::Initialize()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_Window = glfwCreateWindow(1280, 720, "Nightbird", nullptr, nullptr);
	}

	void GlfwPlatform::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void GlfwPlatform::PollEvents()
	{
		glfwPollEvents();
	}

	void GlfwPlatform::WaitEvents()
	{
		glfwWaitEvents();
	}

	bool GlfwPlatform::ShouldClose() const
	{
		return glfwWindowShouldClose(m_Window);
	}

	void GlfwPlatform::GetFramebufferSize(int* width, int* height) const
	{
		glfwGetFramebufferSize(m_Window, width, height);
	}

	GLFWwindow* GlfwPlatform::GetWindow() const
	{
		return m_Window;
	}
}