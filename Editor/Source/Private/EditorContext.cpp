#include "EditorContext.h"

#include "Core/SceneObject.h"

namespace Nightbird::Editor
{
	EditorContext::EditorContext(Core::Engine& engine)
		: m_Engine(engine)
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
}