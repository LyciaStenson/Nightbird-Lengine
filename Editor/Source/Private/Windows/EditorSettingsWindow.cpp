#include "Windows/EditorSettingsWindow.h"

namespace Nightbird::Editor
{
	EditorSettingsWindow::EditorSettingsWindow(EditorSettings& settings)
		: SettingsWindow("Editor Settings")
	{
		SetSettings(settings);
	}
}
