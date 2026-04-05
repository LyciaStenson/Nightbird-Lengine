#include "Windows/EditorSettingsWindow.h"

namespace Nightbird::Editor
{
	NB_OBJECT_IMPL(EditorSettingsWindow, SettingsWindow);

	EditorSettingsWindow::EditorSettingsWindow(EditorSettings& settings)
		: SettingsWindow("Editor Settings")
	{
		SetSettings(settings);
	}
}
