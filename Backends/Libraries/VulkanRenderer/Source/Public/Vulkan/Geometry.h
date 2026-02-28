#pragma once

#include "Vulkan/Buffer.h"

#include <volk.h>

#include <memory>

namespace Nightbird::Core
{
	class MeshPrimitive;
}

namespace Nightbird::Vulkan
{
	class Device;

	class Geometry
	{
	public:
		Geometry(Device* device, const Core::MeshPrimitive& primitive);
		~Geometry() = default;

		Geometry(Geometry&&) = default;
		Geometry& operator=(Geometry&&) = default;

		VkBuffer GetVertexBuffer() const;
		VkBuffer GetIndexBuffer() const;
		uint32_t GetIndexCount() const;

	private:
		std::unique_ptr<Buffer> m_VertexBuffer;
		std::unique_ptr<Buffer> m_IndexBuffer;
		uint32_t m_IndexCount = 0;

		void CreateVertexBuffer(Device* device, const Core::MeshPrimitive& primitive);
		void CreateIndexBuffer(Device* device, const Core::MeshPrimitive& primitive);
	};
}