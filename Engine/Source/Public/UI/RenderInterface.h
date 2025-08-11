#pragma once

#include <RmlUi/Core/RenderInterface.h>

#include <volk.h>
#include <vk_mem_alloc.h>

namespace Nightbird
{
	class UIRenderInterface : public Rml::RenderInterface
	{
	private:
		struct shader_vertex_user_data_t
		{
			// Member objects are order-sensitive to match shader.
			Rml::Matrix4f m_transform;
			Rml::Vector2f m_translate;
		};

		struct texture_data_t
		{
			VkImage m_p_vk_image;
			VkImageView m_p_vk_image_view;
			VkSampler m_p_vk_sampler;
			VkDescriptorSet m_p_vk_descriptor_set;
			VmaAllocation m_p_vma_allocation;
		};

		struct geometry_handle_t
		{
			int m_num_indices;

			VkDescriptorBufferInfo m_p_vertex;
			VkDescriptorBufferInfo m_p_index;
			VkDescriptorBufferInfo m_p_shader;

			// @ this is for freeing our logical blocks for VMA
			// see https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/virtual_allocator.html
			VmaVirtualAllocation m_p_vertex_allocation;
			VmaVirtualAllocation m_p_index_allocation;
			VmaVirtualAllocation m_p_shader_allocation;
		};

		// @ main manager for "allocating" vertex, index, uniform stuff
		class MemoryPool
		{
		public:
			MemoryPool();
			~MemoryPool();

			void Initialize(VkDeviceSize byte_size, VkDeviceSize device_min_uniform_alignment, VmaAllocator p_allocator, VkDevice p_device) noexcept;
			void Shutdown() noexcept;

			bool Alloc_GeneralBuffer(VkDeviceSize size, void** p_data, VkDescriptorBufferInfo* p_out, VmaVirtualAllocation* p_alloc) noexcept;
			bool Alloc_VertexBuffer(uint32_t number_of_elements, uint32_t stride_in_bytes, void** p_data, VkDescriptorBufferInfo* p_out,
				VmaVirtualAllocation* p_alloc) noexcept;
			bool Alloc_IndexBuffer(uint32_t number_of_elements, uint32_t stride_in_bytes, void** p_data, VkDescriptorBufferInfo* p_out,
				VmaVirtualAllocation* p_alloc) noexcept;

			void SetDescriptorSet(uint32_t binding_index, uint32_t size, VkDescriptorType descriptor_type, VkDescriptorSet p_set) noexcept;
			void SetDescriptorSet(uint32_t binding_index, VkDescriptorBufferInfo* p_info, VkDescriptorType descriptor_type,
				VkDescriptorSet p_set) noexcept;
			void SetDescriptorSet(uint32_t binding_index, VkSampler p_sampler, VkImageLayout layout, VkImageView p_view, VkDescriptorType descriptor_type,
				VkDescriptorSet p_set) noexcept;

			void Free_GeometryHandle(geometry_handle_t* p_valid_geometry_handle) noexcept;
			void Free_GeometryHandle_ShaderDataOnly(geometry_handle_t* p_valid_geometry_handle) noexcept;

		private:
			VkDeviceSize m_memory_total_size;
			VkDeviceSize m_device_min_uniform_alignment;
			char* m_p_data;
			VkBuffer m_p_buffer;
			VmaAllocation m_p_buffer_alloc;
			VkDevice m_p_device;
			VmaAllocator m_p_vk_allocator;
			VmaVirtualBlock m_p_block;
		};

		class DescriptorPoolManager
		{
		public:
			DescriptorPoolManager() : m_allocated_descriptor_count{}, m_p_descriptor_pool{} {}
			~DescriptorPoolManager()
			{
				//RMLUI_VK_ASSERTMSG(m_allocated_descriptor_count <= 0, "something is wrong. You didn't free some VkDescriptorSet");
			}

			void Initialize(VkDevice p_device, uint32_t count_uniform_buffer, uint32_t count_image_sampler, uint32_t count_sampler,
				uint32_t count_storage_buffer) noexcept
			{
				//RMLUI_VK_ASSERTMSG(p_device, "you can't pass an invalid VkDevice here");

				Rml::Array<VkDescriptorPoolSize, 5> sizes;
				sizes[0] = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, count_uniform_buffer};
				sizes[1] = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, count_uniform_buffer};
				sizes[2] = {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, count_image_sampler};
				sizes[3] = {VK_DESCRIPTOR_TYPE_SAMPLER, count_sampler};
				sizes[4] = {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, count_storage_buffer};

				VkDescriptorPoolCreateInfo info = {};
				info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				info.pNext = nullptr;
				info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
				info.maxSets = 1000;
				info.poolSizeCount = static_cast<uint32_t>(sizes.size());
				info.pPoolSizes = sizes.data();

				auto status = vkCreateDescriptorPool(p_device, &info, nullptr, &m_p_descriptor_pool);
				//RMLUI_VK_ASSERTMSG(status == VkResult::VK_SUCCESS, "failed to vkCreateDescriptorPool");
			}

			void Shutdown(VkDevice p_device)
			{
				//RMLUI_VK_ASSERTMSG(p_device, "you can't pass an invalid VkDevice here");

				vkDestroyDescriptorPool(p_device, m_p_descriptor_pool, nullptr);
			}

			uint32_t Get_AllocatedDescriptorCount() const noexcept { return m_allocated_descriptor_count; }

			bool Alloc_Descriptor(VkDevice p_device, VkDescriptorSetLayout* p_layouts, VkDescriptorSet* p_sets,
				uint32_t descriptor_count_for_creation = 1) noexcept
			{
				//RMLUI_VK_ASSERTMSG(p_layouts, "you have to pass a valid and initialized VkDescriptorSetLayout (probably you must create it)");
				//RMLUI_VK_ASSERTMSG(p_device, "you must pass a valid VkDevice here");

				VkDescriptorSetAllocateInfo info = {};
				info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				info.pNext = nullptr;
				info.descriptorPool = m_p_descriptor_pool;
				info.descriptorSetCount = descriptor_count_for_creation;
				info.pSetLayouts = p_layouts;

				auto status = vkAllocateDescriptorSets(p_device, &info, p_sets);
				//RMLUI_VK_ASSERTMSG(status == VkResult::VK_SUCCESS, "failed to vkAllocateDescriptorSets");

				m_allocated_descriptor_count += descriptor_count_for_creation;

				return status == VkResult::VK_SUCCESS;
			}

			void Free_Descriptors(VkDevice p_device, VkDescriptorSet* p_sets, uint32_t descriptor_count = 1) noexcept
			{
				//RMLUI_VK_ASSERTMSG(p_device, "you must pass a valid VkDevice here");

				if (p_sets)
				{
					m_allocated_descriptor_count -= descriptor_count;
					vkFreeDescriptorSets(p_device, m_p_descriptor_pool, descriptor_count, p_sets);
				}
			}

		private:
			int m_allocated_descriptor_count;
			VkDescriptorPool m_p_descriptor_pool;
		};

		Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override;

		void RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture) override;

		void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;

		Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override;

		Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override;

		void ReleaseTexture(Rml::TextureHandle texture) override;

		void EnableScissorRegion(bool enable) override;

		void SetScissorRegion(Rml::Rectanglei region) override;

		bool m_is_apply_to_regular_geometry_stencil;
		bool m_is_use_stencil_pipeline;

		VkInstance m_p_instance;
		VkDevice m_p_device;
		VkPhysicalDevice m_p_physical_device;
		VkSurfaceKHR m_p_surface;
		VkSwapchainKHR m_p_swapchain;
		VmaAllocator m_p_allocator;

		VkCommandBuffer m_p_current_command_buffer;

		VkDescriptorSetLayout m_p_descriptor_set_layout_vertex_transform;
		VkDescriptorSetLayout m_p_descriptor_set_layout_texture;
		VkPipelineLayout m_p_pipeline_layout;
		VkPipeline m_p_pipeline_with_textures;
		VkPipeline m_p_pipeline_without_textures;
		VkPipeline m_p_pipeline_stencil_for_region_where_geometry_will_be_drawn;
		VkPipeline m_p_pipeline_stencil_for_regular_geometry_that_applied_to_region_with_textures;
		VkPipeline m_p_pipeline_stencil_for_regular_geometry_that_applied_to_region_without_textures;
		VkDescriptorSet m_p_descriptor_set;
		VkRenderPass m_p_render_pass;
		VkSampler m_p_sampler_linear;
		VkRect2D m_scissor;

		shader_vertex_user_data_t m_user_data_for_vertex_shader;

		MemoryPool m_memory_pool;
		DescriptorPoolManager m_manager_descriptors;
	};
}