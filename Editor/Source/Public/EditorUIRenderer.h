#pragma once

#include <memory>

namespace Nightbird::Editor
{
	class ImGuiPlatform;
	class ImGuiRenderer;

	class EditorUIRenderer
	{
	public:
		EditorUIRenderer(std::unique_ptr<ImGuiPlatform> platform, std::unique_ptr<ImGuiRenderer> renderer);
		~EditorUIRenderer() = default;

		void Initialize();
		void Shutdown();
		void BeginFrame();
		void EndFrame();

	private:
		std::unique_ptr<ImGuiPlatform> m_Platform;
		std::unique_ptr<ImGuiRenderer> m_Renderer;
	};
}
