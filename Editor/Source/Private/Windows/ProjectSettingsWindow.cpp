#include "Windows/ProjectSettingsWindow.h"

namespace Nightbird::Editor
{
	NB_OBJECT_IMPL(ProjectSettingsWindow, ImGuiWindow)

	ProjectSettingsWindow::ProjectSettingsWindow(ProjectSettings& settings)
		: SettingsWindow("Project Settings")
	{
		SetSettings(settings);
	}
}
