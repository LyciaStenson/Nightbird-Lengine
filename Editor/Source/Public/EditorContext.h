#pragma once

namespace Nightbird::Core
{
	class Engine;
	class SceneObject;
}

namespace Nightbird::Editor
{
	class EditorContext
	{
	public:
		EditorContext(Core::Engine& engine);

		void SelectObject(Core::SceneObject* object);
		void ClearSelection();

		Core::SceneObject* GetSelectedObject() const;

		Core::Engine& GetEngine();

	private:
		Core::Engine& m_Engine;
		Core::SceneObject* m_SelectedObject = nullptr;
	};
}