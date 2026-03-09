#include "Vulkan/Geometry.h"

#include "Vulkan/Device.h"
#include "Vulkan/BufferUtils.h"

#include "Core/MeshPrimitive.h"

#include <cstring>

namespace Nightbird::Vulkan
{
	Geometry::Geometry(Device* device, const Core::MeshPrimitive& primitive)
	{
		m_IndexCount = static_cast<uint32_t>(primitive.GetIndices().size());
		CreateVertexBuffer(device, primitive);
		CreateIndexBuffer(device, primitive);
	}

	void Geometry::CreateVertexBuffer(Device* device, const Core::MeshPrimitive& primitive)
	{
		const auto& vertices = primitive.GetVertices();
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		Buffer stagingBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data = stagingBuffer.Map();
		memcpy(data, vertices.data(), (size_t)bufferSize);
		stagingBuffer.Unmap();

		m_VertexBuffer = std::make_unique<Buffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		CopyBuffer(device, stagingBuffer.Get(), m_VertexBuffer->Get(), bufferSize);
	}

	void Geometry::CreateIndexBuffer(Device* device, const Core::MeshPrimitive& primitive)
	{
		const auto& indices = primitive.GetIndices();
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		Buffer stagingBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data = stagingBuffer.Map();
		memcpy(data, indices.data(), (size_t)bufferSize);
		stagingBuffer.Unmap();

		m_IndexBuffer = std::make_unique<Buffer>(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		CopyBuffer(device, stagingBuffer.Get(), m_IndexBuffer->Get(), bufferSize);
	}

	VkBuffer Geometry::GetVertexBuffer() const
	{
		return m_VertexBuffer->Get();
	}

	VkBuffer Geometry::GetIndexBuffer() const
	{
		return m_IndexBuffer->Get();
	}

	uint32_t Geometry::GetIndexCount() const
	{
		return m_IndexCount;
	}
}
