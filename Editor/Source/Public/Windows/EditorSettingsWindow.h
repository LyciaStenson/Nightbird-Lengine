#pragma once

#include "Windows/SettingsWindow.h"

#include "EditorSettings.h"

namespace Nightbird::Editor
{
	class EditorSettingsWindow : public SettingsWindow
	{
	public:
		NB_OBJECT(EditorSettingsWindow, SettingsWindow)

		EditorSettingsWindow(EditorSettings& settings);
	};
}
