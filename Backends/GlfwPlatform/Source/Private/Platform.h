#pragma once

#include "InputProvider.h"
#include "Core/Platform.h"

#include <GLFW/glfw3.h>

#include <memory>

namespace Nightbird::Glfw
{
	class Platform : public Core::Platform
	{
	public:
		virtual void Initialize() override;
		virtual void Shutdown() override;
		virtual void PollEvents() override;
		virtual void WaitEvents() override;
		virtual bool ShouldClose() const override;
		virtual void GetFramebufferSize(int* width, int* height) const override;

		Input::Provider& GetInputProvider() override;

		GLFWwindow* GetWindow() const;

	private:
		std::unique_ptr<Input::Provider> m_InputProvider;

		GLFWwindow* m_Window = nullptr;
	};
}