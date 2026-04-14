#include "Windows/EditorSettingsWindow.h"

namespace Nightbird::Editor
{
	NB_OBJECT_NO_FACTORY_IMPL(Nightbird::Editor::EditorSettingsWindow, Nightbird::Editor::ImGuiWindow)

	EditorSettingsWindow::EditorSettingsWindow(EditorSettings& settings)
		: SettingsWindow("Editor Settings")
	{
		SetSettings(settings);
	}
}
