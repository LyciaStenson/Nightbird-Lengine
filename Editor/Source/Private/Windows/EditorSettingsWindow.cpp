#include "Windows/EditorSettingsWindow.h"

namespace Nightbird::Editor
{
	NB_OBJECT_IMPL(EditorSettingsWindow, ImGuiWindow)

	EditorSettingsWindow::EditorSettingsWindow(EditorSettings& settings)
		: SettingsWindow("Editor Settings")
	{
		SetSettings(settings);
	}
}
