#include "Texture.h"

#include "Device.h"
#include "Buffer.h"
#include "BufferUtils.h"

#include "Core/Texture.h"

#include <iostream>

namespace Nightbird::Vulkan
{
	Texture::Texture(Device* device, const Nightbird::Texture& texture, bool sRGB)
		: m_Device(device)
	{
		CreateFromPixels(texture.GetPixels().data(), texture.GetWidth(), texture.GetHeight(), sRGB);
		CreateTextureSampler();
	}

	Texture::Texture(Device* device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags, VkImageAspectFlags aspectFlags)
		: m_Device(device)
	{
		m_Image = std::make_unique<Image>(device, width, height, format, usageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, aspectFlags);

		if (usageFlags & VK_IMAGE_USAGE_SAMPLED_BIT)
			CreateTextureSampler();
	}

	Texture::~Texture()
	{
		if (m_Sampler != VK_NULL_HANDLE)
			vkDestroySampler(m_Device->GetLogical(), m_Sampler, nullptr);
	}

	VkImageView Texture::GetImageView() const
	{
		return m_Image->GetImageView();
	}

	VkSampler Texture::GetSampler() const
	{
		return m_Sampler;
	}

	void Texture::TransitionToShaderRead(VkCommandBuffer commandBuffer)
	{
		m_Image->TransitionImageLayout(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void Texture::TransitionToColor(VkCommandBuffer commandBuffer)
	{
		m_Image->TransitionImageLayout(commandBuffer, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	}

	void Texture::CreateFromPixels(const uint8_t* pixels, uint32_t width, uint32_t height, bool sRGB)
	{
		VkDeviceSize imageSize = width * height * 4;

		Buffer stagingBuffer(m_Device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data = stagingBuffer.Map();
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		stagingBuffer.Unmap();

		VkFormat format = sRGB ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_SNORM;

		m_Image = std::make_unique<Image>(m_Device, width, height, format, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

		m_Image->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyBufferToImage(m_Device, stagingBuffer.Get(), m_Image->Get(), width, height);
		m_Image->TransitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void Texture::CreateTextureSampler()
	{
		VkPhysicalDeviceProperties deviceProperties{};
		vkGetPhysicalDeviceProperties(m_Device->GetPhysical(), &deviceProperties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = deviceProperties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(m_Device->GetLogical(), &samplerInfo, nullptr, &m_Sampler) != VK_SUCCESS)
			std::cerr << "Failed to create texture sampler" << std::endl;
	}
}