#pragma once

#include <RmlUi/Core/SystemInterface.h>

struct GLFWwindow;

namespace Nightbird
{
	class UISystemInterface : public Rml::SystemInterface
	{
	public:
		UISystemInterface(GLFWwindow* window);

		double GetElapsedTime() override;
		
		void SetClipboardText(const Rml::String& text) override;
		
		void GetClipboardText(Rml::String& text) override;

		bool LogMessage(Rml::Log::Type type, const Rml::String& message) override;

	private:
		GLFWwindow* m_Window = nullptr;
	};
}