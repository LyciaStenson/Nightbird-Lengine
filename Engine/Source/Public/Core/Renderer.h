#pragma once

namespace Nightbird::Core
{
	class Scene;

	class Renderer
	{
	public:
		virtual ~Renderer() = default;

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void SubmitScene(const Scene& scene) = 0;
		virtual void DrawFrame() = 0;
	};
}