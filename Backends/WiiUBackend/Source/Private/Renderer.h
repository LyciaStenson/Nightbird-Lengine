#pragma once

#include "Core/Renderer.h"
#include "Core/Renderable.h"

#include <vector>

namespace Nightbird::Core
{
	class Scene;
}

namespace Nightbird::WiiU
{
	class Renderer : public Core::Renderer
	{
	public:
		void Initialize() override;
		void Shutdown() override;
		void SubmitScene(const Core::Scene& scene) override;
		void DrawFrame() override;

	private:
		std::vector<Core::Renderable> m_Renderables;
	};
}