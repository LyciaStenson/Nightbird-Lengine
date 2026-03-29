#include "Vulkan/Renderer.h"

#include "Core/Platform.h"
#include "Core/Scene.h"
#include "Core/Camera.h"

#include "Vulkan/CameraUBO.h"
#include "Vulkan/LightData.h"
#include "Vulkan/Config.h"

#include "Core/Log.h"

#include <algorithm>

namespace Nightbird::Vulkan
{
	Renderer::Renderer(std::vector<const char*> extensions, SurfaceCreator surfaceCreator)
		: m_Extensions(std::move(extensions)), m_SurfaceCreator(surfaceCreator)
	{

	}

	void Renderer::Initialize()
	{
		m_DefaultTexture = CreateDefaultTexture();

		m_Instance = std::make_unique<Instance>(m_Extensions);
		m_Surface = m_SurfaceCreator(m_Instance->Get());

		m_Device = std::make_unique<Device>(m_Instance->Get(), m_Surface);

		m_SwapChain = std::make_unique<SwapChain>(m_Device.get(), m_Surface);
		m_RenderPass = std::make_unique<RenderPass>(m_Device.get(), m_SwapChain->GetColorFormat(), m_SwapChain->GetDepthFormat(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		m_SwapChain->CreateFramebuffers(m_RenderPass->Get());

		m_Sync = std::make_unique<Sync>(m_Device->GetLogical());

		CreateDescriptorPool();

		m_DescriptorSetLayoutManager = std::make_unique<DescriptorSetLayoutManager>(m_Device.get());

		m_GlobalDescriptorSetManager = std::make_unique<GlobalDescriptorSetManager>(m_Device.get(), m_DescriptorSetLayoutManager->GetGlobalDescriptorSetLayout(), m_DescriptorPool);

		m_OpaquePipeline = std::make_unique<Pipeline>(m_Device.get(), m_RenderPass.get(), m_DescriptorSetLayoutManager.get(), PipelineType::Opaque, false);
		m_TransparentPipeline = std::make_unique<Pipeline>(m_Device.get(), m_RenderPass.get(), m_DescriptorSetLayoutManager.get(), PipelineType::Transparent, false);

		m_TransformPool = std::make_unique<TransformPool>(m_Device.get(), m_DescriptorPool, m_DescriptorSetLayoutManager.get(), 1000);

		Core::Log::Info("Vulkan Renderer Initialized");
	}

	void Renderer::Shutdown()
	{
		vkDeviceWaitIdle(m_Device->GetLogical());

		m_GeometryCache.clear();
		m_MaterialCache.clear();

		m_TransformPool.reset();

		m_OpaquePipeline.reset();
		m_TransparentPipeline.reset();

		m_GlobalDescriptorSetManager.reset();
		m_DescriptorSetLayoutManager.reset();

		vkDestroyDescriptorPool(m_Device->GetLogical(), m_DescriptorPool, nullptr);

		m_Sync.reset();
		m_SwapChain.reset();
		m_RenderPass.reset();
		
		vkDestroySurfaceKHR(m_Instance->Get(), m_Surface, nullptr);

		m_Device.reset();
		m_Instance.reset();
	}

	void Renderer::SubmitScene(const Core::Scene& scene)
	{
		m_ActiveCamera = scene.GetActiveCamera();
		m_Renderables = scene.CollectRenderables();
		m_DirectionalLights = scene.CollectDirectionalLights();
		m_PointLights = scene.CollectPointLights();
	}

	void Renderer::DrawFrame()
	{
		vkWaitForFences(m_Device->GetLogical(), 1, &m_Sync->m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(m_Device->GetLogical(), m_SwapChain->Get(), UINT64_MAX, m_Sync->m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);
		
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			// Should recreate swap chain
			return;
		}
		else if (result != VK_SUCCESS)
		{
			Core::Log::Error("Failed to acquire swap chain image");
			return;
		}

		vkResetFences(m_Device->GetLogical(), 1, &m_Sync->m_InFlightFences[m_CurrentFrame]);

		VkCommandBuffer commandBuffer = m_Device->GetCommandBuffer(m_CurrentFrame);
		vkResetCommandBuffer(commandBuffer, 0);

		m_RenderPass->BeginCommandBuffer(commandBuffer);
		m_RenderPass->Begin(commandBuffer, m_SwapChain->m_Framebuffers[imageIndex], m_SwapChain->m_Extent);

		DrawScene(commandBuffer);

		m_RenderPass->End(commandBuffer);
		m_RenderPass->EndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_Sync->m_ImageAvailableSemaphores[m_CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore signalSemaphores[] = { m_Sync->m_RenderFinishedSemaphores[m_CurrentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, m_Sync->m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS)
		{
			Core::Log::Error("Failed to submit draw command buffer");
			return;
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_SwapChain->Get() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		result = vkQueuePresentKHR(m_Device->GetPresentQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			// Should recreate swap chain
			return;
		}
		else if (result != VK_SUCCESS)
		{
			Core::Log::Error("Failed to present swap chain image");
		}

		m_CurrentFrame = (m_CurrentFrame + 1) % Config::MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::DrawScene(VkCommandBuffer commandBuffer)
	{
		if (!m_ActiveCamera)
			return;

		std::vector<DirectionalLightData> directionalLightData;
		for (const auto* directionalLight : m_DirectionalLights)
		{
			DirectionalLightData data{};
			glm::mat4 worldMatrix = directionalLight->GetWorldMatrix();
			glm::vec3 forward = glm::normalize(glm::vec3(worldMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
			data.direction = glm::vec4(forward, 0.0f);
			data.colorIntensity = glm::vec4(directionalLight->m_Color, directionalLight->m_Intensity);
			directionalLightData.push_back(data);
		}
		m_GlobalDescriptorSetManager->UpdateDirectionalLights(m_CurrentFrame, directionalLightData);
		
		std::vector<PointLightData> pointLightData;
		for (const auto* pointLight : m_PointLights)
		{
			PointLightData data{};
			glm::vec3 worldPos = glm::vec3(pointLight->GetWorldMatrix()[3]);
			data.positionRadius = glm::vec4(worldPos, pointLight->m_Radius);
			data.colorIntensity = glm::vec4(pointLight->m_Color, pointLight->m_Intensity);
			pointLightData.push_back(data);
		}
		m_GlobalDescriptorSetManager->UpdatePointLights(m_CurrentFrame, pointLightData);

		m_TransformPool->Reset();

		CameraUBO cameraUBO{};
		cameraUBO.view = m_ActiveCamera->GetViewMatrix();
		glm::mat4 proj = m_ActiveCamera->GetProjectionMatrix(static_cast<float>(m_SwapChain->m_Extent.width), static_cast<float>(m_SwapChain->m_Extent.height));
		proj[1][1] *= -1;
		cameraUBO.projection = proj;
		cameraUBO.position = glm::vec4(m_ActiveCamera->GetWorldMatrix()[3]);

		m_GlobalDescriptorSetManager->UpdateCamera(m_CurrentFrame, cameraUBO);

		std::vector<const Core::Renderable*> opaqueRenderables;
		std::vector<const Core::Renderable*> transparentRenderables;

		for (const auto& renderable : m_Renderables)
		{
			if (renderable.primitive->GetMaterial()->transparencyEnabled)
				transparentRenderables.push_back(&renderable);
			else
				opaqueRenderables.push_back(&renderable);
		}

		glm::vec3 cameraPos = glm::vec3(m_ActiveCamera->GetWorldMatrix()[3]);
		std::sort(transparentRenderables.begin(), transparentRenderables.end(),
			[&](const Core::Renderable* a, const Core::Renderable* b)
			{
				float distA = glm::length(cameraPos - glm::vec3(a->transform[3]));
				float distB = glm::length(cameraPos - glm::vec3(a->transform[3]));
				return distA > distB;
			}
		);
		
		m_OpaquePipeline->Bind(commandBuffer);
		for (const auto* renderable : opaqueRenderables)
			DrawRenderable(commandBuffer, *renderable, m_OpaquePipeline.get());

		m_TransparentPipeline->Bind(commandBuffer);
		for (const auto* renderable : transparentRenderables)
			DrawRenderable(commandBuffer, *renderable, m_TransparentPipeline.get());
	}

	void Renderer::DrawRenderable(VkCommandBuffer commandBuffer, const Core::Renderable& renderable, Pipeline* currentPipeline)
	{
		Geometry& geometry = GetOrCreateGeometry(renderable.primitive);
		Material& material = GetOrCreateMaterial(renderable.primitive->GetMaterial().get());

		VkDescriptorSet transformDescriptorSet = m_TransformPool->Acquire(renderable.transform, m_CurrentFrame);
		
		VkBuffer vertexBuffers[] = { geometry.GetVertexBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, geometry.GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);

		std::array<VkDescriptorSet, 3> descriptorSets =
		{
			m_GlobalDescriptorSetManager->GetDescriptorSets()[m_CurrentFrame], transformDescriptorSet, material.GetDescriptorSets()[m_CurrentFrame]
		};

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, currentPipeline->GetLayout(), 0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);

		vkCmdDrawIndexed(commandBuffer, geometry.GetIndexCount(), 1, 0, 0, 0);
	}

	void Renderer::CreateDescriptorPool()
	{
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = 10000;

		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = 10000;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 10000;

		if (vkCreateDescriptorPool(m_Device->GetLogical(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
		{
			Core::Log::Error("Failed to create descriptor pool");
		}
	}

	std::shared_ptr<Core::Texture> Renderer::CreateDefaultTexture()
	{
		std::vector<uint8_t> pixels = { 255, 255, 255, 255 };
		return std::make_shared<Core::Texture>(1, 1, Core::TextureFormat::RGBA8, pixels);
	}

	Geometry& Renderer::GetOrCreateGeometry(const Core::MeshPrimitive* primitive)
	{
		auto it = m_GeometryCache.find(primitive);
		if (it != m_GeometryCache.end())
			return it->second;

		m_GeometryCache.emplace(primitive, Geometry(m_Device.get(), *primitive));
		return m_GeometryCache.at(primitive);
	}
	
	Material& Renderer::GetOrCreateMaterial(const Core::Material* material)
	{
		auto it = m_MaterialCache.find(material);
		if (it != m_MaterialCache.end())
			return it->second;

		m_MaterialCache.emplace(material, Material(m_Device.get(), *material, m_DescriptorPool, m_DescriptorSetLayoutManager.get(), *m_DefaultTexture));
		return m_MaterialCache.at(material);
	}
}
