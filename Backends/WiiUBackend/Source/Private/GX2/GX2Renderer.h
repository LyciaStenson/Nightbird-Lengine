#pragma once

#include "Core/Renderer.h"

#include "Core/Renderable.h"
#include "Core/Texture.h"

#include "GX2/GX2Geometry.h"
#include "GX2/GX2Material.h"

#include <gx2/shaders.h>
#include <whb/gfx.h>

#include <vector>
#include <unordered_map>
#include <memory>

namespace Nightbird::Core
{
	class Scene;
	class Camera;
	class MeshPrimitive;
	struct Material;
}

namespace Nightbird::GX2
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
		Material& GetOrCreateMaterial(const Core::Material* material);

		std::vector<Core::Renderable> m_Renderables;
		Core::Camera* m_ActiveCamera = nullptr;
		std::unordered_map<const Core::MeshPrimitive*, GX2::Geometry> m_GeometryCache;
		std::unordered_map<const Core::Material*, GX2::Material> m_MaterialCache;

		std::shared_ptr<Core::Texture> m_DefaultTexture;

		WHBGfxShaderGroup m_ShaderGroup = {};

		uint32_t m_CameraBlockLocation = 0;
		uint32_t m_ModelBlockLocation = 0;

		float* m_CameraData = nullptr;
		float* m_ModelData = nullptr;
	};
}
