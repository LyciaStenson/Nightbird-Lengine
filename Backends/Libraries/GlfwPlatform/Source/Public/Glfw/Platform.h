#pragma once

#include "Core/Platform.h"

#include "Glfw/InputProvider.h"
#include "Glfw/AudioProvider.h"

#include <GLFW/glfw3.h>

#include <vector>
#include <memory>
#include <string>

namespace Nightbird::Glfw
{
	class Platform : public Core::Platform
	{
	public:
		virtual void Initialize() override;
		virtual void Shutdown() override;
		virtual void Update() override;
		virtual void WaitEvents() override;
		virtual bool ShouldClose() const override;
		virtual void GetFramebufferSize(int* width, int* height) const override;

		virtual std::string GetCookedAssetsPath() const override;

		Input::Provider& GetInputProvider() override;
		Audio::Provider& GetAudioProvider() override;

		std::vector<const char*> GetRequiredExtensions() const;

		GLFWwindow* GetWindow() const;

	private:
		std::unique_ptr<Input::Provider> m_InputProvider;
		AudioProvider m_AudioProvider;

		GLFWwindow* m_Window = nullptr;
	};
}
