#pragma once

#include "Vulkan/Image.h"

#include <array>
#include <memory>

namespace Nightbird::Vulkan
{
	class Device;
	
	class Cubemap
	{
	public:
		Cubemap(Device* device, uint32_t faceSize, const std::array<const uint8_t*, 6>& faceData);
		~Cubemap();

		VkImageView GetImageView() const;
		VkSampler GetSampler() const;

	private:
		std::unique_ptr<Image> m_Image;

		VkSampler m_Sampler = VK_NULL_HANDLE;

		Device* m_Device = nullptr;
	};
}