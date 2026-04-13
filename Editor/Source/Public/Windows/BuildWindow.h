#pragma once

#include "ImGuiWindow.h"

namespace Nightbird::Editor
{
	class EditorContext;

	class BuildWindow : public ImGuiWindow
	{
	public:
		NB_OBJECT(BuildWindow, ImGuiWindow)

		BuildWindow(EditorContext& context, bool open = false);

	protected:
		void OnRender() override;

	private:
		EditorContext& m_Context;
	};
}
