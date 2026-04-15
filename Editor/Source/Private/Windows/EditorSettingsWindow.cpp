#include "Windows/EditorSettingsWindow.h"

namespace Nightbird::Editor
{
	NB_REFLECT_NO_FIELDS(EditorSettingsWindow, NB_PARENT(ImGuiWindow), NB_NO_FACTORY)

	EditorSettingsWindow::EditorSettingsWindow(EditorSettings& settings)
		: SettingsWindow("Editor Settings")
	{
		//SetSettings(settings);
	}
}
