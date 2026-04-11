#pragma once

namespace Nightbird::Core
{
	class Engine;
	class SceneObject;
}

namespace Nightbird::Editor
{
	class EditorUIBackend;
	class ImportManager;

	class EditorContext
	{
	public:
		EditorContext(Core::Engine& engine, EditorUIBackend& editorUIBackend, ImportManager& importManager);

		void SelectObject(Core::SceneObject* object);
		void ClearSelection();

		Core::SceneObject* GetSelectedObject() const;

		Core::Engine& GetEngine();

		EditorUIBackend& GetEditorUIBackend() const;

		ImportManager& GetImportManager() const;

	private:
		Core::Engine& m_Engine;
		EditorUIBackend& m_EditorUIBackend;
		ImportManager& m_ImportManager;
		
		Core::SceneObject* m_SelectedObject = nullptr;
	};
}