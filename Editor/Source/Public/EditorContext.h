#pragma once

namespace Nightbird::Core
{
	class Engine;
	class SceneObject;
}

namespace Nightbird::Editor
{
	class EditorUIBackend;

	class EditorContext
	{
	public:
		EditorContext(Core::Engine& engine, EditorUIBackend& editorUIBackend);

		void SelectObject(Core::SceneObject* object);
		void ClearSelection();

		Core::SceneObject* GetSelectedObject() const;

		Core::Engine& GetEngine();

		EditorUIBackend& GetEditorUIBackend() const;

	private:
		Core::Engine& m_Engine;
		EditorUIBackend& m_EditorUIBackend;
		
		Core::SceneObject* m_SelectedObject = nullptr;
	};
}