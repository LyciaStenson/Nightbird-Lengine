#include "ImGuiWindow.h"

namespace Nightbird::Editor
{
	ImGuiWindow::ImGuiWindow(const std::string& title, bool open, const ImGuiWindowConfig& config)
		: m_Title(std::move(title)), m_Open(open), m_Config(std::move(config))
	{

	}
	
	void ImGuiWindow::Render()
	{
		if (!m_Open)
			return;

		if (m_Config.size)
			ImGui::SetNextWindowSize(*m_Config.size, ImGuiCond_Once);
		if (m_Config.padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, *m_Config.padding);

		if (ImGui::Begin(m_Title.c_str(), &m_Open, m_Config.flags))
		{
			OnRender();
		}

		ImGui::End();

		if (m_Config.padding)
			ImGui::PopStyleVar();
	}

	void ImGuiWindow::SetOpen(bool open)
	{
		m_Open = open;
	}

	bool ImGuiWindow::IsOpen() const
	{
		return m_Open;
	}

	const std::string& ImGuiWindow::GetTitle() const
	{
		return m_Title;
	}
}
