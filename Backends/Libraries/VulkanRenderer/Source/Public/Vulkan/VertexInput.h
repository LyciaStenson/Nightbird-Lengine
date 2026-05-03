#pragma once

#include <volk.h>

#include <array>

namespace Nightbird::Vulkan
{
	struct VertexInput
	{
		static VkVertexInputBindingDescription GetBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 5> GetAttributeDescriptions();
	};
}