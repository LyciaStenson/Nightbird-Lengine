#pragma once

namespace Nightbird::Core
{
	class Scene;
	class Camera;
	class RenderSurface;

	class Renderer
	{
	public:
		virtual ~Renderer() = default;

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual RenderSurface& GetDefaultSurface() = 0;
		virtual void SubmitScene(const Scene& scene, const Camera& camera) = 0;
		virtual void BeginFrame(RenderSurface& surface) = 0;
		virtual void EndFrame(RenderSurface& surface) = 0;
		virtual void DrawScene(RenderSurface& surface) = 0;
	};
}
