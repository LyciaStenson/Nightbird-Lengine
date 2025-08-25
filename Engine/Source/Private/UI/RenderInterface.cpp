#include <UI/RenderInterface.h>
#include <UI/ShadersCompiledSPV.h>

#include <iostream>

#include <Core/Renderer.h>
#include <Vulkan/Device.h>
#include <Vulkan/RenderPass.h>

namespace Nightbird
{
	UIRenderInterface::UIRenderInterface(Renderer* renderer)
		: m_Renderer(renderer)
	{
		CreateShaders();
		CreateDescriptorSetLayouts();
		CreatePipelineLayout();
		CreatePipeline();
		CreateDescriptorPool();
		CreateDescriptorSets();
	}

	UIRenderInterface::~UIRenderInterface()
	{
		VkDevice device = m_Renderer->GetDevice()->GetLogical();

		vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);

		vkDestroyPipeline(device, m_Pipeline, nullptr);

		vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);

		vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayoutVertexTransform, nullptr);
		vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayoutTexture, nullptr);

		for (const auto& p_module : m_Shaders)
		{
			vkDestroyShaderModule(device, p_module, nullptr);
		}
		
		for (size_t i = 0; i < m_Geometries.size(); ++i)
			ReleaseGeometry(static_cast<Rml::CompiledGeometryHandle>(i));
	}
	
	void UIRenderInterface::BeginFrame(VkCommandBuffer commandBuffer)
	{
		m_CommandBuffer = commandBuffer;
	}

	void UIRenderInterface::EndFrame()
	{
		m_CommandBuffer = VK_NULL_HANDLE;
	}

	void UIRenderInterface::CreateShaders()
	{
		struct shader_data_t
		{
			const uint32_t* m_data;
			size_t m_data_size;
			VkShaderStageFlagBits m_shader_type;
		};

		const Rml::Vector<shader_data_t> shaders =
		{
			{reinterpret_cast<const uint32_t*>(shader_vert), sizeof(shader_vert), VK_SHADER_STAGE_VERTEX_BIT},
			{reinterpret_cast<const uint32_t*>(shader_frag_color), sizeof(shader_frag_color), VK_SHADER_STAGE_FRAGMENT_BIT},
			{reinterpret_cast<const uint32_t*>(shader_frag_texture), sizeof(shader_frag_texture), VK_SHADER_STAGE_FRAGMENT_BIT},
		};

		VkDevice device = m_Renderer->GetDevice()->GetLogical();

		for (const shader_data_t& shader_data : shaders)
		{
			VkShaderModuleCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			info.pCode = shader_data.m_data;
			info.codeSize = shader_data.m_data_size;

			VkShaderModule p_module = nullptr;
			vkCreateShaderModule(device, &info, nullptr, &p_module);
			
			m_Shaders.push_back(p_module);
		}
	}

	void UIRenderInterface::CreateDescriptorSetLayouts()
	{
		VkDevice device = m_Renderer->GetDevice()->GetLogical();
		
		VkDescriptorSetLayoutBinding vertexTransformBinding = {};
		vertexTransformBinding.binding = 1;
		vertexTransformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		vertexTransformBinding.descriptorCount = 1;
		vertexTransformBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		
		VkDescriptorSetLayoutCreateInfo vertexTransformInfo = {};
		vertexTransformInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		vertexTransformInfo.pBindings = &vertexTransformBinding;
		vertexTransformInfo.bindingCount = 1;
		
		if (vkCreateDescriptorSetLayout(device, &vertexTransformInfo, nullptr, &m_DescriptorSetLayoutVertexTransform) != VK_SUCCESS)
			std::cerr << "Failed to create vertex transform descriptor set layout" << std::endl;
		
		VkDescriptorSetLayoutBinding fragmentTextureBinding = {};
		fragmentTextureBinding.binding = 2;
		fragmentTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		fragmentTextureBinding.descriptorCount = 1;
		fragmentTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		
		VkDescriptorSetLayoutCreateInfo fragmentTextureInfo = {};
		fragmentTextureInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		fragmentTextureInfo.pBindings = &fragmentTextureBinding;
		fragmentTextureInfo.bindingCount = 1;
		
		if (vkCreateDescriptorSetLayout(device, &fragmentTextureInfo, nullptr, &m_DescriptorSetLayoutTexture) != VK_SUCCESS)
			std::cerr << "Failed to create fragment texture descriptor set layout" << std::endl;
	}

	void UIRenderInterface::CreateDescriptorSets()
	{
		VkDevice device = m_Renderer->GetDevice()->GetLogical();

		VkDescriptorSetAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocateInfo.descriptorPool = m_DescriptorPool;
		allocateInfo.descriptorSetCount = 1;
		allocateInfo.pSetLayouts = &m_DescriptorSetLayoutVertexTransform;

		if (vkAllocateDescriptorSets(device, &allocateInfo, &m_DescriptorSetVertexTransform) != VK_SUCCESS)
			std::cerr << "Failed to allocate descriptor set for vertex transform" << std::endl;

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = m_VertexUniformBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(Rml::Matrix4f);

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = m_DescriptorSetVertexTransform;
		write.dstBinding = 1;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		write.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
	}

	void UIRenderInterface::CreateDescriptorPool()
	{
		VkDevice device = m_Renderer->GetDevice()->GetLogical();

		VkDescriptorPoolSize poolSizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10 }
		};

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.maxSets = 10;
		poolInfo.poolSizeCount = 2;
		poolInfo.pPoolSizes = poolSizes;

		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
			std::cerr << "Failed to create descriptor pool" << std::endl;
	}

	void UIRenderInterface::CreatePipelineLayout()
	{
		VkDevice device = m_Renderer->GetDevice()->GetLogical();

		std::array<VkDescriptorSetLayout, 2> layouts =
		{
			m_DescriptorSetLayoutVertexTransform,
			m_DescriptorSetLayoutTexture
		};
		
		VkPipelineLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
		layoutInfo.pSetLayouts = layouts.data();
		layoutInfo.pushConstantRangeCount = 0;
		layoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
			std::cerr << "Failed to create pipeline layout" << std::endl;
	}

	void UIRenderInterface::CreatePipeline()
	{
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
		rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationInfo.lineWidth = 1.0f;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
		colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendInfo.attachmentCount = 1;
		colorBlendInfo.pAttachments = &colorBlendAttachment;

		VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
		depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilInfo.depthTestEnable = VK_FALSE;
		depthStencilInfo.depthWriteEnable = VK_FALSE;
		depthStencilInfo.stencilTestEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.scissorCount = 1;

		VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.dynamicStateCount = 2;
		dynamicStateInfo.pDynamicStates = dynamicStates;

		VkPipelineMultisampleStateCreateInfo multisampleInfo{};
		multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkVertexInputBindingDescription vertexBindingDescription{};
		vertexBindingDescription.binding = 0;
		vertexBindingDescription.stride = sizeof(Rml::Vertex);
		vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::array<VkVertexInputAttributeDescription, 3> vertexAttributes{};
		vertexAttributes[0].binding = 0;
		vertexAttributes[0].location = 0;
		vertexAttributes[0].format = VK_FORMAT_R32G32_SFLOAT;
		vertexAttributes[0].offset = offsetof(Rml::Vertex, position);

		vertexAttributes[1].binding = 0;
		vertexAttributes[1].location = 1;
		vertexAttributes[1].format = VK_FORMAT_R8G8B8A8_UNORM;
		vertexAttributes[1].offset = offsetof(Rml::Vertex, colour);

		vertexAttributes[2].binding = 0;
		vertexAttributes[2].location = 2;
		vertexAttributes[2].format = VK_FORMAT_R32G32_SFLOAT;
		vertexAttributes[2].offset = offsetof(Rml::Vertex, tex_coord);

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size());
		vertexInputInfo.pVertexAttributeDescriptions = vertexAttributes.data();
		
		VkPipelineShaderStageCreateInfo vertexShaderInfo{};
		vertexShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexShaderInfo.module = m_Shaders[static_cast<int>(ShaderId::Vertex)];
		vertexShaderInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragmentShaderInfo{};
		fragmentShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragmentShaderInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragmentShaderInfo.module = m_Shaders[static_cast<int>(ShaderId::FragmentColor)];
		fragmentShaderInfo.pName = "main";

		std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
		shaderStages[0] = vertexShaderInfo;
		shaderStages[1] = fragmentShaderInfo;
		
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportInfo;
		pipelineInfo.pRasterizationState = &rasterizationInfo;
		pipelineInfo.pMultisampleState = &multisampleInfo;
		pipelineInfo.pColorBlendState = &colorBlendInfo;
		pipelineInfo.pDepthStencilState = &depthStencilInfo;
		pipelineInfo.pDynamicState = &dynamicStateInfo;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = m_Renderer->GetRenderPass()->Get();
		pipelineInfo.subpass = 0;

		if (vkCreateGraphicsPipelines(m_Renderer->GetDevice()->GetLogical(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline))
			std::cerr << "Failed to create graphics pipeline" << std::endl;
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

		m_Geometries.push_back(geometry);
		return static_cast<Rml::CompiledGeometryHandle>(m_Geometries.size() - 1);
	}

	/// Called by RmlUi when it wants to render application-compiled geometry.
	void UIRenderInterface::RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture)
	{
		if (m_CommandBuffer == VK_NULL_HANDLE)
			return;

		Geometry& geometry = m_Geometries[static_cast<size_t>(handle)];

		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &geometry.vertexBuffer, offsets);
		vkCmdBindIndexBuffer(m_CommandBuffer, geometry.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

		// Need to bind pipeline and descriptor sets
		//vkCmdDrawIndexed(m_CommandBuffer, geometry.indexCount, 1, 0, 0, 0);
	}

	/// Called by RmlUi when it wants to release application-compiled geometry.
	void UIRenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle handle)
	{
		size_t index = static_cast<size_t>(handle);
		if (index >= m_Geometries.size())
			return;

		VmaAllocator allocator = m_Renderer->GetDevice()->GetAllocator();
		Geometry& geometry = m_Geometries[index];

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
		//std::cout << "RmlUi wants to generate texture" << std::endl;

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