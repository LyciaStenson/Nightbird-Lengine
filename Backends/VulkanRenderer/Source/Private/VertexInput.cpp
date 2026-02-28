#include "VertexInput.h"

#include "Core/Vertex.h"

namespace Nightbird::Vulkan
{
	VkVertexInputBindingDescription VertexInput::GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Core::Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	std::array<VkVertexInputAttributeDescription, 5> VertexInput::GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};

		// Position attribute
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Core::Vertex, position);

		// Normal attribute
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Core::Vertex, normal);

		// Base color tex coord attribute
		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Core::Vertex, baseColorTexCoord);

		// Metallic roughness tex coord attribute
		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Core::Vertex, metallicRoughnessTexCoord);

		// Normal tex coord attribute
		attributeDescriptions[4].binding = 0;
		attributeDescriptions[4].location = 4;
		attributeDescriptions[4].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[4].offset = offsetof(Core::Vertex, normalTexCoord);

		return attributeDescriptions;
	}
}