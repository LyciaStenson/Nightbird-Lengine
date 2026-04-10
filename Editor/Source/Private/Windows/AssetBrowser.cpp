#include "Windows/AssetBrowser.h"

#include "EditorContext.h"

#include "Core/Log.h"

namespace Nightbird::Editor
{
	AssetBrowser::AssetBrowser(EditorContext& context, bool open)
		: ImGuiWindow("Asset Browser", open), m_Context(context)
	{
		m_CurrentPath = std::filesystem::path("Assets");
	}

	void AssetBrowser::OnRender()
	{
		if (m_CurrentPath.has_parent_path())
		{
			if (ImGui::Button("Up"))
			{
				m_CurrentPath = m_CurrentPath.parent_path();
				m_SelectedPath.clear();
			}
		}

		for (const auto& entry : std::filesystem::directory_iterator(m_CurrentPath))
		{
			const auto& path = entry.path();
			const std::string name = path.filename().string();

			bool selected = path == m_SelectedPath;

			if (entry.is_directory())
			{
				if (ImGui::Selectable(name.c_str(), selected))
				{
					m_SelectedPath = path;
				}
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					m_CurrentPath = path;
					m_SelectedPath.clear();
				}
			}
			else if (entry.is_regular_file())
			{
				if (ImGui::Selectable(name.c_str(), selected))
				{
					m_SelectedPath = path;
				}
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					Core::Log::Info("AssetBrowser: Open file: " + path.string());
				}
			}
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			m_SelectedPath.clear();
		}
	}
}