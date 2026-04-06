#include "Windows/SceneWindow.h"

#include "EditorContext.h"

#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "Core/Log.h"

namespace Nightbird::Editor
{
	NB_OBJECT_IMPL(SceneWindow, ImGuiWindow);

	SceneWindow::SceneWindow(EditorContext& context, bool open)
		: ImGuiWindow("Scene", open, {ImGuiWindowFlags_MenuBar, ImVec2(300, 500)}), m_Context(context)
	{

	}

	void SceneWindow::OnRender()
	{

	}
}
