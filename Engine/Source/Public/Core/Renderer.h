#pragma once

#include <memory>
#include <cstdint>

namespace Nightbird::Core
{
	class Scene;
	class Camera;
	class RenderSurface;
	class OffscreenSurface;

	enum class PixelFormat
	{
		RGBA8,
		RGBA16F,
		Depth24Stencil8,
		Depth32F
	};

	class Renderer
	{
	public:
		virtual ~Renderer() = default;

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual RenderSurface& GetDefaultSurface() = 0;
		virtual std::unique_ptr<RenderSurface> CreateSurface(uint32_t width, uint32_t height, PixelFormat colorFormat, PixelFormat depthFormat) = 0;
		virtual void SubmitScene(const Scene& scene, const Camera& camera) = 0;
		virtual bool BeginFrame(RenderSurface& surface) = 0;
		virtual void EndFrame(RenderSurface& surface) = 0;
		virtual void DrawScene(RenderSurface& surface) = 0;
	};
}
