#pragma once

#include "Core/Renderer.h"

#include "Core/Renderable.h"

#include "PICA/Geometry.h"

#include <citro3d.h>

#include <vector>
#include <unordered_map>

namespace Nightbird::Core
{
	class Scene;
	class Camera;
	class MeshPrimitive;
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

		Geometry& GetOrCreateGeometry(const Core::MeshPrimitive* primitive);

		Core::Camera* m_ActiveCamera = nullptr;
		std::vector<Core::Renderable> m_Renderables;
		std::unordered_map<const Core::MeshPrimitive*, Geometry> m_GeometryCache;

		C3D_RenderTarget* m_TopTarget = nullptr;

		DVLB_s* m_ShaderDvlb = nullptr;
		shaderProgram_s m_ShaderProgram = {};
		int m_ULocProjection = -1;
		int m_ULocModelView = -1;
	};
}
