#pragma once

#include "Core/Renderer.h"

namespace Nightbird
{
	class WiiURenderer : public Renderer
	{
	public:
		void Initialize() override;
		void Shutdown() override;
		void DrawFrame() override;
	};
}