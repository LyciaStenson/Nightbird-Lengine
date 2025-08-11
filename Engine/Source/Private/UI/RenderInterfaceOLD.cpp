#include <UI/RenderInterface.h>

#include <iostream>

#include <RmlUi/Core.h>

static int nextHandle = 1;

namespace Nightbird
{
	Rml::CompiledGeometryHandle UIRenderInterface::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices)
	{
		std::cout << "CompileGeometry" << std::endl;
		return nextHandle++;
	}

	void UIRenderInterface::RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture)
	{
		std::cout << "RenderGeometry" << std::endl;

		RMLUI_ZoneScopedN("Vulkan - RenderCompiledGeometry");

		if (m_p_current_command_buffer == VK_NULL_HANDLE)
			return;

		//RMLUI_VK_ASSERTMSG(m_p_current_command_buffer, "must be valid otherwise you can't render now!!! (can't be)");

		texture_data_t* p_texture = reinterpret_cast<texture_data_t*>(texture);

		VkDescriptorImageInfo info_descriptor_image = {};
		if (p_texture && p_texture->m_p_vk_descriptor_set == nullptr)
		{
			VkDescriptorSet p_texture_set = nullptr;
			m_manager_descriptors.Alloc_Descriptor(m_p_device, &m_p_descriptor_set_layout_texture, &p_texture_set);

			info_descriptor_image.imageView = p_texture->m_p_vk_image_view;
			info_descriptor_image.sampler = p_texture->m_p_vk_sampler;
			info_descriptor_image.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkWriteDescriptorSet info_write = {};

			info_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			info_write.dstSet = p_texture_set;
			info_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			info_write.dstBinding = 2;
			info_write.pImageInfo = &info_descriptor_image;
			info_write.descriptorCount = 1;

			vkUpdateDescriptorSets(m_p_device, 1, &info_write, 0, nullptr);
			p_texture->m_p_vk_descriptor_set = p_texture_set;
		}

		geometry_handle_t* p_casted_compiled_geometry = reinterpret_cast<geometry_handle_t*>(geometry);

		m_user_data_for_vertex_shader.m_translate = translation;

		VkDescriptorSet p_current_descriptor_set = nullptr;
		p_current_descriptor_set = m_p_descriptor_set;

		//RMLUI_VK_ASSERTMSG(p_current_descriptor_set,
			//"you can't have here an invalid pointer of VkDescriptorSet. Two reason might be. 1. - you didn't allocate it "
			//"at all or 2. - Somehing is wrong with allocation and somehow it was corrupted by something.");

		shader_vertex_user_data_t* p_data = nullptr;

		if (p_casted_compiled_geometry->m_p_shader_allocation == nullptr)
		{
			// it means it was freed in ReleaseCompiledGeometry method
			bool status = m_memory_pool.Alloc_GeneralBuffer(sizeof(m_user_data_for_vertex_shader), reinterpret_cast<void**>(&p_data),
				&p_casted_compiled_geometry->m_p_shader, &p_casted_compiled_geometry->m_p_shader_allocation);
			//RMLUI_VK_ASSERTMSG(status, "failed to allocate VkDescriptorBufferInfo for uniform data to shaders");
		}
		else
		{
			// it means our state is dirty and we need to update data, but it is not right in terms of architecture, for real better experience would
			// be great to free all "compiled" geometries and "re-build" them in one general way, but here I got only three callings for
			// font-face-layer textures (load_document example) and that shit. So better to think how to make it right, if it is fine okay, if it is
			// not okay and like we really expect that ReleaseCompiledGeometry for all objects that needs to be rebuilt so better to implement that,
			// but still it is a big architectural thing (or at least you need to do something big commits here to implement a such feature), so my
			// implementation doesn't break anything what we had, but still it looks strange. If I get callings for releasing maybe I need to use it
			// for all objects not separately????? Otherwise it is better to provide method for resizing (or some kind of "resizing" callback) for
			// recalculating all geometry IDK, so it means you pass the existed geometry that wasn't pass to ReleaseCompiledGeometry, but from another
			// hand you need to re-build compiled geometry again so we have two kinds of geometry one is compiled and never changes and one is dynamic
			// and it goes through pipeline InitializationOfProgram...->Compile->Render->Release->Compile->Render->Release...

			m_memory_pool.Free_GeometryHandle_ShaderDataOnly(p_casted_compiled_geometry);
			bool status = m_memory_pool.Alloc_GeneralBuffer(sizeof(m_user_data_for_vertex_shader), reinterpret_cast<void**>(&p_data),
				&p_casted_compiled_geometry->m_p_shader, &p_casted_compiled_geometry->m_p_shader_allocation);
			//RMLUI_VK_ASSERTMSG(status, "failed to allocate VkDescriptorBufferInfo for uniform data to shaders");
		}

		if (p_data)
		{
			p_data->m_transform = m_user_data_for_vertex_shader.m_transform;
			p_data->m_translate = m_user_data_for_vertex_shader.m_translate;
		}
		//else
		//{
			//RMLUI_VK_ASSERTMSG(p_data, "you can't reach this zone, it means something bad");
		//}

		const uint32_t pDescriptorOffsets = static_cast<uint32_t>(p_casted_compiled_geometry->m_p_shader.offset);

		VkDescriptorSet p_texture_descriptor_set = nullptr;

		if (p_texture)
		{
			p_texture_descriptor_set = p_texture->m_p_vk_descriptor_set;
		}

		VkDescriptorSet p_sets[] = {p_current_descriptor_set, p_texture_descriptor_set};
		int real_size_of_sets = 2;

		if (p_texture == nullptr)
			real_size_of_sets = 1;

		vkCmdBindDescriptorSets(m_p_current_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_p_pipeline_layout, 0, real_size_of_sets, p_sets, 1,
			&pDescriptorOffsets);

		if (m_is_use_stencil_pipeline)
		{
			vkCmdBindPipeline(m_p_current_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_p_pipeline_stencil_for_region_where_geometry_will_be_drawn);
		}
		else
		{
			if (p_texture)
			{
				if (m_is_apply_to_regular_geometry_stencil)
				{
					vkCmdBindPipeline(m_p_current_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
						m_p_pipeline_stencil_for_regular_geometry_that_applied_to_region_with_textures);
				}
				else
				{
					vkCmdBindPipeline(m_p_current_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_p_pipeline_with_textures);
				}
			}
			else
			{
				if (m_is_apply_to_regular_geometry_stencil)
				{
					vkCmdBindPipeline(m_p_current_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
						m_p_pipeline_stencil_for_regular_geometry_that_applied_to_region_without_textures);
				}
				else
				{
					vkCmdBindPipeline(m_p_current_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_p_pipeline_without_textures);
				}
			}
		}

		vkCmdBindVertexBuffers(m_p_current_command_buffer, 0, 1, &p_casted_compiled_geometry->m_p_vertex.buffer,
			&p_casted_compiled_geometry->m_p_vertex.offset);

		vkCmdBindIndexBuffer(m_p_current_command_buffer, p_casted_compiled_geometry->m_p_index.buffer, p_casted_compiled_geometry->m_p_index.offset,
			VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(m_p_current_command_buffer, p_casted_compiled_geometry->m_num_indices, 1, 0, 0, 0);
	}

	void UIRenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry)
	{
		std::cout << "ReleaseGeometry" << std::endl;
	}

	Rml::TextureHandle UIRenderInterface::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source)
	{
		std::cout << "LoadTexture" << std::endl;
		return 0;
	}

	Rml::TextureHandle UIRenderInterface::GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions)
	{
		std::cout << "GenerateTexture" << std::endl;
		return 0;
	}

	void UIRenderInterface::ReleaseTexture(Rml::TextureHandle texture)
	{
		std::cout << "ReleaseTexture" << std::endl;
	}

	void UIRenderInterface::EnableScissorRegion(bool enable)
	{
		std::cout << "EnableScissorRegion" << std::endl;
	}

	void UIRenderInterface::SetScissorRegion(Rml::Rectanglei region)
	{
		std::cout << "SetScissorRegion" << std::endl;
	}
}