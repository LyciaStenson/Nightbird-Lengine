#include "Windows/BuildWindow.h"

#include "EditorContext.h"

namespace Nightbird::Editor
{
	NB_OBJECT_IMPL(BuildWindow, ImGuiWindow)

	BuildWindow::BuildWindow(EditorContext& context, bool open)
	: ImGuiWindow("Build Window", open, { ImGuiWindowFlags_NoDocking }), m_Context(context)
	{

	}

	void BuildWindow::OnRender()
	{

	}
}
