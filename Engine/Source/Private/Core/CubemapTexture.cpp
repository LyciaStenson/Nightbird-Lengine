#include "Core/CubemapTexture.h"

namespace Nightbird::Core
{
	CubemapTexture::CubemapTexture(uint32_t faceSize)
		: m_FaceSize(faceSize)
	{

	}

	uint32_t CubemapTexture::GetFaceSize() const
	{
		return m_FaceSize;
	}
}