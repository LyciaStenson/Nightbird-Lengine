#pragma once

#include <cstdint>

namespace Nightbird::Core
{
	class CubemapTexture
	{
	public:
		CubemapTexture(uint32_t faceSize);

		uint32_t GetFaceSize() const;

	private:
		uint32_t m_FaceSize;
	};
}