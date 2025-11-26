#include "Windows/AboutWindow.h"

namespace Nightbird
{
	AboutWindow::AboutWindow(bool open)
		: ImGuiWindow("About", open, BuildProperties())
	{

	}

	void AboutWindow::OnRender()
	{
		ImGui::Text("Nightbird Engine alpha 0.1.0");
	}

	ImGuiWindowProperties AboutWindow::BuildProperties()
	{
		ImGuiWindowProperties properties;
		properties.fixedSize = true;
		properties.size = {360, 140};
		properties.enableDocking = false;
		
		return properties;
	}
}