#pragma once

namespace Nightbird
{
	class Renderer
	{
	public:
		virtual ~Renderer() = default;

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void DrawFrame() = 0;
	};
}