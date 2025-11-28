#pragma once

#include <vector>

#include "ImGuiWindow.h"

namespace Nightbird
{
	class SceneManager;
	
	class CreateObjectWindow : public ImGuiWindow
	{
	public:
		CreateObjectWindow(SceneManager* sceneManager, bool open = false);

	protected:
		void OnRender() override;
		
		static ImGuiWindowProperties BuildProperties();
		
		SceneManager* m_Scene;
	};
}