#pragma once

#include "ImGuiWindow.h"

namespace Nightbird
{
	class ModelManager;
	class SceneManager;
	
	class InstantiateModelWindow : public ImGuiWindow
	{
	public:
		InstantiateModelWindow(ModelManager* modelManager, SceneManager* sceneManager, bool open = false);

	protected:
		void OnRender() override;

		static ImGuiWindowProperties BuildProperties();

		ModelManager* m_ModelManager = nullptr;
		SceneManager* m_Scene = nullptr;
	};
}