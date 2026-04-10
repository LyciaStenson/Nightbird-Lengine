#include "Windows/AboutWindow.h"

namespace Nightbird::Editor
{
	AboutWindow::AboutWindow(bool open)
	: ImGuiWindow("About Nightbird", open, { ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize, ImVec2(512, 448) })
	{

	}

	void AboutWindow::OnRender()
	{
		ImGui::Text("Nightbird-Lengine dev-0.1.0");

		ImGui::Dummy(ImVec2(0.0f, 10));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 10));

		ImGui::TextWrapped("MIT License\n\nCopyright (c) 2026 NightbirdEngine\n\nPermission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n\nThe above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");

		ImVec2 buttonSize(72, 22);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

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
