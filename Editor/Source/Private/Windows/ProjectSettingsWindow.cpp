#include "Windows/ProjectSettingsWindow.h"

namespace Nightbird::Editor
{
	//NB_OBJECT_NO_FACTORY_IMPL(Nightbird::Editor::ProjectSettingsWindow, Nightbird::Editor::ImGuiWindow)

	ProjectSettingsWindow::ProjectSettingsWindow(ProjectSettings& settings)
		: SettingsWindow("Project Settings")
	{
		SetSettings(settings);
	}
}
