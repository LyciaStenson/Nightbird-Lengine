#pragma once

#include "Core/Renderer.h"

namespace Nightbird::WiiU
{
	class Renderer : public Nightbird::Renderer
	{
	public:
		void Initialize() override;
		void Shutdown() override;
		void DrawFrame() override;
	};
}