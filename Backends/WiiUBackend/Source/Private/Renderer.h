#pragma once

#include "Core/Renderer.h"

#include "Core/Renderable.h"

#include <gx2/shaders.h>
#include <gx2r/buffer.h>
#include <whb/gfx.h>

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
		void DrawScene();

		std::vector<Core::Renderable> m_Renderables;

		WHBGfxShaderGroup m_ShaderGroup = {};

		GX2RBuffer m_PositionBuffer = {};
	};
}
