#pragma once

#include <GLFW/glfw3.h>

#include <RmlUi/Core/SystemInterface.h>

namespace Nightbird
{
	class UISystemInterface : public Rml::SystemInterface
	{
	public:
		explicit UISystemInterface(GLFWwindow* window);

		double GetElapsedTime() override;

	private:
		GLFWwindow* m_Window = nullptr;
	};
}