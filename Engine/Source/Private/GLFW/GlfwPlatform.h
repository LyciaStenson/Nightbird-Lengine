#pragma once

#include "Core/Platform.h"

#include <GLFW/glfw3.h>

namespace Nightbird
{
	class GlfwPlatform : public Platform
	{
	public:
		virtual void Initialize() override;
		virtual void Shutdown() override;
		virtual void PollEvents() override;
		virtual void WaitEvents() override;
		virtual bool ShouldClose() const override;
		virtual void GetFramebufferSize(int* width, int* height) const override;

		GLFWwindow* GetWindow() const;

	private:
		GLFWwindow* m_Window = nullptr;
	};
}