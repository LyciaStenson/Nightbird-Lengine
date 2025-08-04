#include <LoadModelWindow.h>

#include <memory>
#include <string>

#include <Core/ModelManager.h>
#include <Core/Model.h>

namespace Nightbird
{
	LoadModelWindow::LoadModelWindow(ModelManager* modelManager, bool open)
		: ImGuiWindow("Load Model", open, BuildProperties()), m_ModelManager(modelManager)
	{

	}

	void LoadModelWindow::OnRender()
	{
		static std::string path;
		static char pathBuffer[1024];

		strncpy_s(pathBuffer, path.c_str(), sizeof(pathBuffer));
		pathBuffer[sizeof(pathBuffer) - 1] = '\0';

		ImGui::Text("Load a .glb format model.");

		if (ImGui::InputText("Path", pathBuffer, sizeof(pathBuffer)))
		{
			path = std::string(pathBuffer);
		}

		ImGui::BeginDisabled(path.size() < 1);
		if (ImGui::Button("Load Model"))
		{
			m_ModelManager->LoadModelAsync(path, [this](std::shared_ptr<Model> model)
				{
					SetOpen(false);
				});
		}
		ImGui::EndDisabled();
	}

	ImGuiWindowProperties LoadModelWindow::BuildProperties()
	{
		ImGuiWindowProperties properties;
		properties.fixedSize = true;
		properties.size = {400, 600};
		properties.enableDocking = false;

		return properties;
	}
}