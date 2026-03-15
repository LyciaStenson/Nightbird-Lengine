#pragma once

#include "Core/Renderer.h"

#include "Core/Renderable.h"

#include "Geometry.h"

#include <gx2/shaders.h>
#include <whb/gfx.h>

#include <vector>
#include <unordered_map>

namespace Nightbird::Core
{
	class Scene;
	class Camera;
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
		void DrawScene(float width, float height);

		Geometry& GetOrCreateGeometry(const Core::MeshPrimitive* primitive);

		std::vector<Core::Renderable> m_Renderables;
		Core::Camera* m_ActiveCamera = nullptr;
		std::unordered_map<const Core::MeshPrimitive*, Geometry> m_GeometryCache;

		WHBGfxShaderGroup m_ShaderGroup = {};

		uint32_t m_CameraBlockLocation = 0;
		uint32_t m_ModelBlockLocation = 0;

		float* m_CameraData = nullptr;
		float* m_ModelData = nullptr;
	};
}
