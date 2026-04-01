#pragma once

#include "GX2/GX2RenderSurface.h"

namespace Nightbird::GX2
{
	class RenderSurfaceDRC : public RenderSurface
	{
	public:
		RenderSurfaceDRC();

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;

		void Begin() override;
		void Finish() override;
	};
}
