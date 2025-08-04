#pragma once

#include <ImGuiWindow.h>

namespace Nightbird
{
	class AboutWindow : public ImGuiWindow
	{
	public:
		AboutWindow(bool open = false);

	protected:
		void OnRender() override;

		static ImGuiWindowProperties BuildProperties();
	};
}