#include <Core/GlfwWindow.h>

#include <iostream>

#include <GLFW/glfw3.h>

#include <Core/Renderer.h>

namespace Nightbird
{
	GlfwWindow::GlfwWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_Window = glfwCreateWindow(1280, 720, "Nightbird", nullptr, nullptr);
		glfwSetFramebufferSizeCallback(m_Window, FramebufferResizeCallback);
	}

	GlfwWindow::~GlfwWindow()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	GLFWwindow* GlfwWindow::Get() const
	{
		return m_Window;
	}

	void GlfwWindow::SetUserPointer(Renderer* renderer)
	{
		m_UserData = {renderer};
		glfwSetWindowUserPointer(m_Window, &m_UserData);
	}
	
	void GlfwWindow::GetFramebufferSize(int* width, int* height)
	{
		glfwGetFramebufferSize(m_Window, width, height);
	}

	void GlfwWindow::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto userData = static_cast<WindowUserData*>(glfwGetWindowUserPointer(window));
		if (userData && userData->renderer)
			userData->renderer->FramebufferResized();
	}
}