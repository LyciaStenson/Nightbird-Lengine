#pragma once

#include <cstdint>
#include <vector>

namespace Nightbird
{
	class Texture
	{
	public:
		Texture(uint32_t width, uint32_t height, std::vector<uint8_t> pixels);

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		const std::vector<uint8_t>& GetPixels() const;

	private:
		uint32_t m_Width;
		uint32_t m_Height;
		std::vector<uint8_t> m_Pixels;
	};
}