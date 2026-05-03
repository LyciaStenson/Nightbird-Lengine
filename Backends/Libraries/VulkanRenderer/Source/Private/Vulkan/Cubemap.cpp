#include "Vulkan/Cubemap.h"

#include "Vulkan/Device.h"

namespace Nightbird::Vulkan
{
	Cubemap::Cubemap(Device* device, uint32_t faceSize, const std::array<const uint8_t*, 6>& faceData)
	{

	}

	Cubemap::~Cubemap()
	{
		if (m_Sampler != VK_NULL_HANDLE)
			vkDestroySampler(m_Device->GetLogical(), m_Sampler, nullptr);
	}

	VkImageView Cubemap::GetImageView() const
	{
		return m_Image->GetImageView();
	}

	VkSampler Cubemap::GetSampler() const
	{
		return m_Sampler;
	}
}