#include "Windows/AboutWindow.h"

NB_REFLECT_NO_FIELDS(Nightbird::Editor::AboutWindow, NB_PARENT(Nightbird::Editor::ImGuiWindow), NB_NO_FACTORY)

namespace Nightbird::Editor
{
	AboutWindow::AboutWindow(bool open)
		: ImGuiWindow("About Nightbird", open, { ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse, ImVec2(512, 480) })
	{

	}

	void AboutWindow::OnRender()
	{
		ImGui::Text("Nightbird Engine");

		ImGui::Dummy(ImVec2(0.0f, 1.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 1.0f));

		ImGui::Text("dev-0.1.0");

		ImGui::Dummy(ImVec2(0.0f, 1.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 1.0f));

		ImVec2 buttonSize(72.0f, 22.0f);
		float spacing = ImGui::GetStyle().ItemSpacing.y;
		float reserve = buttonSize.y + spacing;

		if (ImGui::BeginTabBar("Licenses"))
		{
			if (ImGui::BeginTabItem("License"))
			{
				ImGui::BeginChild("License", ImVec2(0.0f, -reserve), true);
				ImGui::TextWrapped("MIT License\n\nCopyright (c) 2026 NightbirdEngine\n\nPermission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n\nThe above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
				ImGui::EndChild();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Vendor"))
			{
				ImGui::BeginChild("Vendor", ImVec2(0.0f, -reserve), true);
				ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
				ImGui::EndChild();

				ImGui::EndTabItem();
			}
			
			ImGui::EndTabBar();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

		ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

		ImGui::SetCursorPos(ImVec2(
			contentMax.x - buttonSize.x,
			contentMax.y - buttonSize.y
		));
		
		if (ImGui::Button("OK", buttonSize))
			SetOpen(false);

		ImGui::PopStyleVar();
	}
}
