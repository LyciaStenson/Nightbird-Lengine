#include "EditorContext.h"

#include "EditorUIBackend.h"

#include "Core/SceneObject.h"

namespace Nightbird::Editor
{
	EditorContext::EditorContext(Core::Engine& engine, EditorUIBackend& editorUIBackend)
		: m_Engine(engine), m_EditorUIBackend(editorUIBackend)
	{

	}

	void EditorContext::SelectObject(Core::SceneObject* object)
	{
		m_SelectedObject = object;
	}

	void EditorContext::ClearSelection()
	{
		m_SelectedObject = nullptr;
	}

	Core::SceneObject* EditorContext::GetSelectedObject() const
	{
		return m_SelectedObject;
	}

	Core::Engine& EditorContext::GetEngine()
	{
		return m_Engine;
	}

	EditorUIBackend& EditorContext::GetEditorUIBackend() const
	{
		return m_EditorUIBackend;
	}
}