#pragma once

#include "Core/Renderer.h"

#include "Core/Renderable.h"

#include <citro3d.h>

#include <vector>

namespace Nightbird::Core
{
	class Scene;
	class Camera;
}

namespace Nightbird::PICA
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
		Core::Camera* m_ActiveCamera = nullptr;

		C3D_RenderTarget* m_TopTarget = nullptr;

		DVLB_s* m_ShaderDvlb = nullptr;
		shaderProgram_s m_ShaderProgram = {};
		int m_ULocProjection = -1;
		int m_ULocModelView = -1;

		void* m_VertexBuffer = nullptr;
	};
}
