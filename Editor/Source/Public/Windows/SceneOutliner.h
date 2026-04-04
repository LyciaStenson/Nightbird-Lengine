#pragma once

#include "ImGuiWindow.h"

namespace Nightbird::Core
{
	class SceneObject;
}

namespace Nightbird::Editor
{
	class EditorContext;

	class SceneOutliner : public ImGuiWindow
	{
	public:
		SceneOutliner(EditorContext& context, bool open = true);

	protected:
		void OnRender() override;

	private:
		void DrawSceneNode(Core::SceneObject* object);

		EditorContext& m_Context;
	};
}