#include "Windows/ProjectSettingsWindow.h"

namespace Nightbird::Editor
{
	NB_REFLECT_NO_FIELDS(ProjectSettingsWindow, NB_PARENT(ImGuiWindow), NB_NO_FACTORY)

	ProjectSettingsWindow::ProjectSettingsWindow(ProjectSettings& settings)
		: SettingsWindow("Project Settings")
	{
		//SetSettings(settings);
	}
}
