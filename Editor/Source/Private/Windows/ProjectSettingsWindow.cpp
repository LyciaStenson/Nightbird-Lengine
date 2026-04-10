#include "Windows/ProjectSettingsWindow.h"

namespace Nightbird::Editor
{
	ProjectSettingsWindow::ProjectSettingsWindow(ProjectSettings& settings)
		: SettingsWindow("Project Settings")
	{
		SetSettings(settings);
	}
}
