#include "Vulkan/CubemapTexture.h"

#include "Vulkan/Device.h"

namespace Nightbird::Vulkan
{
	CubemapTexture::CubemapTexture(Device* device, uint32_t faceSize, const std::array<const uint8_t*, 6>& faceData)
	{

	}

	CubemapTexture::~CubemapTexture()
	{
		if (m_Sampler != VK_NULL_HANDLE)
			vkDestroySampler(m_Device->GetLogical(), m_Sampler, nullptr);
	}

	VkImageView CubemapTexture::GetImageView() const
	{
		return m_Image->GetImageView();
	}

	VkSampler CubemapTexture::GetSampler() const
	{
		return m_Sampler;
	}
}