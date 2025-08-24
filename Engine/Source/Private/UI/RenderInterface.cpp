#include <UI/RenderInterface.h>

#include <iostream>

#include <Core/Renderer.h>
#include <Vulkan/Device.h>

namespace Nightbird
{
	UIRenderInterface::UIRenderInterface(Renderer* renderer)
		: m_Renderer(renderer)
	{

	}

	UIRenderInterface::~UIRenderInterface()
	{
		for (size_t i = 0; i < geometries.size(); ++i)
			ReleaseGeometry(static_cast<Rml::CompiledGeometryHandle>(i));
	}
	
	/// Called by RmlUi when it wants to compile geometry it believes will be static for the forseeable future.
	Rml::CompiledGeometryHandle UIRenderInterface::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices)
	{
		std::cout << "RmlUi wants to compile geometry" << std::endl;

		VmaAllocator allocator = m_Renderer->GetDevice()->GetAllocator();

		Geometry geometry{};
		geometry.indexCount = static_cast<uint32_t>(indices.size());

		VkBufferCreateInfo vertexInfo{};
		vertexInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertexInfo.size = sizeof(Rml::Vertex) * vertices.size();
		vertexInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

		VmaAllocationCreateInfo vertexAllocInfo{};
		vertexAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		
		vmaCreateBuffer(allocator, &vertexInfo, &vertexAllocInfo, &geometry.vertexBuffer, &geometry.vertexAllocation, nullptr);

		void* data;
		vmaMapMemory(allocator, geometry.vertexAllocation, &data);
		memcpy(data, vertices.data(), sizeof(Rml::Vertex) * vertices.size());
		vmaUnmapMemory(allocator, geometry.vertexAllocation);

		VkBufferCreateInfo indexInfo{};
		indexInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		indexInfo.size = sizeof(int) * indices.size();
		indexInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

		VmaAllocationCreateInfo indexAllocInfo{};
		indexAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		
		vmaCreateBuffer(allocator, &indexInfo, &indexAllocInfo, &geometry.indexBuffer, &geometry.indexAllocation, nullptr);
		
		vmaMapMemory(allocator, geometry.indexAllocation, &data);
		memcpy(data, indices.data(), sizeof(int) * indices.size());
		vmaUnmapMemory(allocator, geometry.indexAllocation);

		geometries.push_back(geometry);
		return static_cast<Rml::CompiledGeometryHandle>(geometries.size() - 1);
	}

	/// Called by RmlUi when it wants to render application-compiled geometry.
	void UIRenderInterface::RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture)
	{
		std::cout << "RmlUi wants to render geometry" << std::endl;
	}

	/// Called by RmlUi when it wants to release application-compiled geometry.
	void UIRenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle handle)
	{
		size_t index = static_cast<size_t>(handle);
		if (index >= geometries.size())
			return;

		VmaAllocator allocator = m_Renderer->GetDevice()->GetAllocator();
		Geometry& geometry = geometries[index];

		if (geometry.vertexBuffer != VK_NULL_HANDLE)
			vmaDestroyBuffer(allocator, geometry.vertexBuffer, geometry.vertexAllocation);
		if (geometry.indexBuffer != VK_NULL_HANDLE)
			vmaDestroyBuffer(allocator, geometry.indexBuffer, geometry.indexAllocation);

		geometry = Geometry{};
	}

	/// Called by RmlUi when a texture is required by the library.
	Rml::TextureHandle UIRenderInterface::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source)
	{
		std::cout << "RmlUi wants to load texture" << std::endl;
		return Rml::TextureHandle();
	}

	/// Called by RmlUi when a texture is required to be built from an internally-generated sequence of pixels.
	Rml::TextureHandle UIRenderInterface::GenerateTexture(Rml::Span<const Rml::byte> source_data, Rml::Vector2i source_dimensions)
	{
		std::cout << "RmlUi wants to generate texture" << std::endl;

		return Rml::TextureHandle();
	}
	
	/// Called by RmlUi when a loaded texture is no longer required.
	void UIRenderInterface::ReleaseTexture(Rml::TextureHandle texture_handle)
	{
		std::cout << "RmlUi wants to release texture" << std::endl;
	}

	/// Called by RmlUi when it wants to enable or disable scissoring to clip content.
	void UIRenderInterface::EnableScissorRegion(bool enable)
	{
		std::cout << "RmlUi wants to set scissor region enabled to " << enable << std::endl;
		m_Renderer->EnableScissor(enable);
	}

	/// Called by RmlUi when it wants to change the scissor region.
	void UIRenderInterface::SetScissorRegion(Rml::Rectanglei region)
	{
		std::cout << "RmlUi wants to set scissor region" << std::endl;
		
		VkRect2D rect{};
		rect.offset.x = std::max(0, region.Left());
		rect.offset.y = std::max(0, region.Top());
		rect.extent.width = static_cast<uint32_t>(region.Width());
		rect.extent.height = static_cast<uint32_t>(region.Height());
		m_Renderer->SetScissorRect(rect);
	}

	/// Called by RmlUi when it wants to set the current transform matrix to a new matrix.
	void UIRenderInterface::SetTransform(const Rml::Matrix4f* transform)
	{

	}
}