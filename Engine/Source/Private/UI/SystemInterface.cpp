#include <UI/SystemInterface.h>

#include <iostream>

#include <GLFW/glfw3.h>

namespace Nightbird
{
	UISystemInterface::UISystemInterface(GLFWwindow* window)
		: m_Window(window)
	{

	}

	double UISystemInterface::GetElapsedTime()
	{
		return glfwGetTime();
	}

	void UISystemInterface::SetClipboardText(const Rml::String& text)
	{
		glfwSetClipboardString(m_Window, text.c_str());
	}

	void UISystemInterface::GetClipboardText(Rml::String& text)
	{
		const char* clipboard = glfwGetClipboardString(m_Window);
		text = clipboard ? clipboard : "";
	}

	bool UISystemInterface::LogMessage(Rml::Log::Type type, const Rml::String& message)
	{
		std::cout << "[RmlUi] " << message.c_str() << std::endl;
		return true;
	}
}