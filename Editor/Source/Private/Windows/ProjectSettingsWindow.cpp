#include "Windows/ProjectSettingsWindow.h"

namespace Nightbird::Editor
{
	NB_OBJECT_IMPL(ProjectSettingsWindow, SettingsWindow);

	ProjectSettingsWindow::ProjectSettingsWindow(ProjectSettings& settings)
		: SettingsWindow("Project Settings")
	{
		SetSettings(settings);
	}
}
