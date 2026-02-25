#pragma once

#include "Core/Renderer.h"

namespace Nightbird
{
	class VulkanRenderer : public Renderer
	{
	public:
		void Initialize() override;
		void Shutdown() override;
		void DrawFrame() override;
	};
}