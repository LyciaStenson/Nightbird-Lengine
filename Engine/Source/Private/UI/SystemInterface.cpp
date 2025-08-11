#include <UI/SystemInterface.h>

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
}