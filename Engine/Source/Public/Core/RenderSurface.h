#pragma once

#include <cstdint>

namespace Nightbird::Core
{
	class RenderSurface
	{
	public:
		virtual ~RenderSurface() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
	};
}
