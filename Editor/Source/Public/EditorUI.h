#pragma once

namespace Nightbird::Editor
{
	class WindowManager;

	class EditorUI
	{
	public:
		EditorUI(WindowManager& windowManager);

		void Render();

	private:
		void RenderMainMenuBar();

		WindowManager& m_WindowManager;
	};
}