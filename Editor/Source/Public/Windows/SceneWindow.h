#pragma once

#include "ImGuiWindow.h"

namespace Nightbird::Editor
{
	class EditorContext;

	class SceneWindow : public ImGuiWindow
	{
	public:
		NB_OBJECT(SceneWindow, ImGuiWindow)
		SceneWindow(EditorContext& context, bool open = true);

	protected:
		void OnRender() override;

	private:
		EditorContext& m_Context;
	};
}