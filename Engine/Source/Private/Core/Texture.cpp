#include "Core/Texture.h"

namespace Nightbird
{
	Texture::Texture(uint32_t width, uint32_t height, std::vector<uint8_t> pixels)
		: m_Width(width), m_Height(height), m_Pixels(std::move(pixels))
	{

	}

	uint32_t Texture::GetWidth() const
	{
		return m_Width;
	}

	uint32_t Texture::GetHeight() const
	{
		return m_Height;
	}

	const std::vector<uint8_t>& Texture::GetPixels() const
	{
		return m_Pixels;
	}
}