#pragma once

#include <vector>

#include "ImGuiWindow.h"

namespace Nightbird
{
	class Scene;
	
	class CreateObjectWindow : public ImGuiWindow
	{
	public:
		CreateObjectWindow(Scene* scene, bool open = false);

	protected:
		void OnRender() override;
		
		static ImGuiWindowProperties BuildProperties();
		
		Scene* m_Scene;
	};
}