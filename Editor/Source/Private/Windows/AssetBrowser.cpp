#include "Windows/AssetBrowser.h"

#include "EditorContext.h"

#include "Scene/TextSceneReader.h"
#include "Import/ImportManager.h"

#include "Core/Engine.h"
#include "Core/Platform.h"
#include "Core/Scene.h"
#include "Core/Log.h"

NB_REFLECT_NO_FIELDS(Nightbird::Editor::AssetBrowser, NB_PARENT(Nightbird::Editor::ImGuiWindow), NB_NO_FACTORY)

namespace Nightbird::Editor
{
	AssetBrowser::AssetBrowser(EditorContext& context, bool open)
		: ImGuiWindow("Asset Browser", open), m_Context(context)
	{
		m_CurrentPath = context.GetImportManager().GetAssetsDir();
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

				if (ImGui::BeginDragDropSource())
				{
					const AssetInfo* assetInfo = m_Context.GetImportManager().GetAssetInfo(m_SelectedPath);
					if (assetInfo)
					{
						ImGui::SetDragDropPayload("ASSET_UUID", &assetInfo->uuid, sizeof(uuids::uuid));
						ImGui::Text(name.c_str());
					}
					ImGui::EndDragDropSource();
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					const AssetInfo* assetInfo = m_Context.GetImportManager().GetAssetInfo(m_SelectedPath);
					if (assetInfo)
					{
						Core::SceneReadResult result = m_Context.GetImportManager().LoadScene(assetInfo->uuid);
						if (result.root)
						{
							auto scene = std::make_unique<Core::Scene>();
							scene->SetActiveCamera(result.activeCamera);
							
							for (auto& child : result.root->GetChildren())
								scene->GetRoot()->AddChild(std::move(child));

							m_Context.GetEngine().SetScene(std::move(scene));
						}
					}
				}
			}
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
		{
			m_SelectedPath.clear();
		}
	}
}
