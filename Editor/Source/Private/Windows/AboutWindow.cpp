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
			if (ImGui::BeginTabItem("Nightbird"))
			{
				ImGui::BeginChild("License", ImVec2(0.0f, -reserve), true);
				ImGui::TextWrapped("MIT License\n\nCopyright (c) 2026 NightbirdEngine\n\nPermission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n\nThe above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
				ImGui::EndChild();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Vendor"))
			{
				ImGui::BeginChild("Vendor", ImVec2(0.0f, -reserve), true);
				if (ImGui::CollapsingHeader("CafeGLSL"))
				{
					ImGui::TextWrapped("For original Mesa code see https://docs.mesa3d.org/license.html\nAny additions by this fork are licensed under MIT. Thanks to exjam for granting permission to use Decaf's libgfd code.");
				}
				if (ImGui::CollapsingHeader("Dear ImGui"))
				{
					ImGui::TextWrapped("The MIT License (MIT)\n\nCopyright (c) 2014-2026 Omar Cornut\n\nPermission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n\nThe above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. ");
				}
				if (ImGui::CollapsingHeader("dr_libs"))
				{
					ImGui::TextWrapped("Copyright 2025 David Reid\n\nPermission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the \"Software\"), to deal in	the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so.\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
				}
				if (ImGui::CollapsingHeader("fastgltf"))
				{
					ImGui::TextWrapped("The MIT License\nCopyright (c) 2022 - 2025 Sean Apeler.\nAll rights reserved.\nPermission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\nThe above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
				}
				if (ImGui::CollapsingHeader("GLFW"))
				{
					ImGui::TextWrapped("Copyright (c) 2002-2006 Marcus Geelnard\nCopyright(c) 2006 - 2019 Camilla Löwy\nThis software is provided 'as-is', without any express or implied warranty.In no event will the authors be held liable for any damages arising from the use of this software. Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions :\n1.  The origin of this software must not be misrepresented; you must not claim that you wrote the original software.If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.2.  Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.\n3.  This notice may not be removed or altered from any source distribution.");
				}
				if (ImGui::CollapsingHeader("toml++"))
				{
					ImGui::TextWrapped("MIT License\n\nCopyright (c) Mark Gillard <mark.gillard@outlook.com.au>\n\nPermission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n\nThe above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. ");
				}
				if (ImGui::CollapsingHeader("VMA"))
				{
					ImGui::TextWrapped("Copyright (c) 2017-2025 Advanced Micro Devices, Inc. All rights reserved.\n\nPermission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\n\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
				}
				if (ImGui::CollapsingHeader("volk"))
				{
					ImGui::TextWrapped("Copyright (c) 2018-2024 Arseny Kapoulkine\nPermission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
				}
				if (ImGui::CollapsingHeader("Vulkan-Headers"))
				{
					ImGui::TextWrapped("Copyright 2015-2023 The Khronos Group Inc.\n\nFiles in this repository fall under one of these licenses:\n\n- `Apache-2.0`\n- `MIT`\n\nNote: With the exception of `parse_dependency.py` the files using `MIT` license also fall under `Apache-2.0`. Example:\n\n```\nSPDX-License-Identifier: Apache-2.0 OR MIT\n```\n\nFull license text of these licenses is available at:\n\n  * Apache-2.0: https://opensource.org/licenses/Apache-2.0\n\n  * MIT: https://opensource.org/licenses/MIT");
				}
				ImGui::EndChild();

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("External"))
			{
				ImGui::BeginChild("External", ImVec2(0.0f, -reserve), true);
				if (ImGui::CollapsingHeader("wut"))
				{
					ImGui::TextWrapped("Copyright (c) 2015-present devkitPro, wut Authors\n\nThis software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.\n\nPermission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:\n\n1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.\n2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.\n3. This notice may not be removed or altered from any source distribution.");
				}
				if (ImGui::CollapsingHeader("libctru"))
				{
					ImGui::TextWrapped("This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.\n\nPermission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:\n\n1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.\n2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.\n3. This notice may not be removed or altered from any source distribution. ");
				}
				if (ImGui::CollapsingHeader("citro3d"))
				{
					ImGui::TextWrapped("Copyright (C) 2014-2018 fincs\n\nThis software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.\n\nPermission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:\n\n1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.\n2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.\n3. This notice may not be removed or altered from any source distribution.");
				}
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
