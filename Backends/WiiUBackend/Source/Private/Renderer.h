#pragma once

#include "Core/Renderer.h"

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
	};
}