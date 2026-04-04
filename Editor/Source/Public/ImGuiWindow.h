#pragma once

#include <imgui.h>

#include <optional>
#include <string>

namespace Nightbird
{
	struct ImGuiWindowConfig
	{
		ImGuiWindowFlags flags = 0;
		std::optional<ImVec2> size;
		std::optional<ImVec2> padding;
	};

	class ImGuiWindow
	{
	public:
		ImGuiWindow(const std::string& title, bool open = true, const ImGuiWindowConfig& config = {});
		virtual ~ImGuiWindow() = default;

		void Render();
		void SetOpen(bool open);
		bool IsOpen() const;
		const std::string& GetTitle() const;

	protected:
		virtual void OnRender() = 0;

	private:
		std::string m_Title;
		bool m_Open;
		ImGuiWindowConfig m_Config;
	};
}