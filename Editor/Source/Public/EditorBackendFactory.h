#pragma once

#include <memory>

namespace Nightbird::Core
{
	class Platform;
	class Renderer;
}

namespace Nightbird::Editor
{
	class EditorUIRenderer;

	std::unique_ptr<EditorUIRenderer> CreateEditorUIRenderer(Nightbird::Core::Platform& platform, Nightbird::Core::Renderer& renderer);
}
