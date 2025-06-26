#include <ImGui/AboutWindow.h>

using namespace Nightbird;

AboutWindow::AboutWindow(bool open)
	: ImGuiWindow("About", open)
{

}

void AboutWindow::OnRender()
{
	ImGui::Text("Nightbird Engine 1.0.0");
}