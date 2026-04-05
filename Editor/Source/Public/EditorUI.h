#pragma once

#include "EditorTheme.h"

namespace Nightbird::Editor
{
	class WindowManager;

	class EditorUI
	{
	public:
		EditorUI(WindowManager& windowManager);

		void ApplyTheme(EditorTheme theme);

		void Render();

	private:
		void MainMenuBar();
		void Toolbar();
		void DockSpace();

		WindowManager& m_WindowManager;
	};
}
