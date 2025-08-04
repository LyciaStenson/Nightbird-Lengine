#pragma once

#include <string>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace Nightbird
{
	struct ImGuiWindowProperties
	{
		bool hasMenuBar = false;
		bool fixedSize = false;
		ImVec2 size;
		bool changePadding = false;
		ImVec2 padding;
		bool enableDocking = true;
	};

	class ImGuiWindow
	{
	public:
		ImGuiWindow(const std::string& title, bool open = true, const ImGuiWindowProperties& properties = {});
		virtual ~ImGuiWindow() = default;

		void SetOpen(bool open);

		void Render();

	protected:
		virtual void OnRender() = 0;

		std::string m_Title;
		bool m_Open;
		bool m_HasMenuBar;
		bool m_FixedSize;
		ImVec2 m_Size;
		bool m_ChangePadding;
		ImVec2 m_Padding;
		bool m_EnableDocking;
	};
}