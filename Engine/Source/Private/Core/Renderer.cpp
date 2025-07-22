#include <Core/Renderer.h>

#include <iostream>

#include <Vulkan/Config.h>
#include <Vulkan/Device.h>
#include <Vulkan/Instance.h>
#include <Vulkan/SwapChain.h>
#include <Vulkan/RenderPass.h>
#include <Vulkan/DescriptorSetLayoutManager.h>
#include <Vulkan/GlobalDescriptorSetManager.h>
#include <Vulkan/Pipeline.h>
#include <Vulkan/DescriptorPool.h>
#include <Vulkan/Sync.h>
#include <Core/GlfwWindow.h>
#include <Core/Scene.h>
#include <Core/SceneObject.h>
#include <Core/Renderable.h>
#include <Core/Mesh.h>
#include <Core/MeshPrimitive.h>
#include <Core/MeshInstance.h>
#include <Core/Camera.h>
#include <Core/RenderTarget.h>

namespace Nightbird
{
	Renderer::Renderer(GlfwWindow* glfwWindow)
		: glfwWindow(glfwWindow)
	{
		instance = std::make_unique<VulkanInstance>(glfwWindow->Get());
		device = std::make_unique<VulkanDevice>(instance->Get(), instance->GetSurface());

		swapChain = std::make_unique<VulkanSwapChain>(device.get(), instance->GetSurface(), glfwWindow->Get());
		renderPass = std::make_unique<VulkanRenderPass>(device.get(), swapChain->GetColorFormat(), FindDepthFormat(device->GetPhysical()), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		swapChain->CreateFramebuffers(renderPass->Get());

		descriptorSetLayoutManager = std::make_unique<VulkanDescriptorSetLayoutManager>(device.get());

		descriptorPool = std::make_unique<VulkanDescriptorPool>(device.get(), 1024);

		globalDescriptorSetManager = std::make_unique<GlobalDescriptorSetManager>(device.get(), descriptorSetLayoutManager->GetGlobalDescriptorSetLayout(), descriptorPool->Get());

		opaquePipeline = std::make_unique<VulkanPipeline>(device.get(), renderPass.get(), descriptorSetLayoutManager.get(), globalDescriptorSetManager.get(), PipelineType::Opaque, false);
		transparentPipeline = std::make_unique<VulkanPipeline>(device.get(), renderPass.get(), descriptorSetLayoutManager.get(), globalDescriptorSetManager.get(), PipelineType::Transparent, false);
		opaqueDoubleSidedPipeline = std::make_unique<VulkanPipeline>(device.get(), renderPass.get(), descriptorSetLayoutManager.get(), globalDescriptorSetManager.get(), PipelineType::Opaque, true);
		transparentDoubleSidedPipeline = std::make_unique<VulkanPipeline>(device.get(), renderPass.get(), descriptorSetLayoutManager.get(), globalDescriptorSetManager.get(), PipelineType::Transparent, true);

		opaquePipeline->SetDescriptorPool(descriptorPool->Get());
		transparentPipeline->SetDescriptorPool(descriptorPool->Get());
		opaqueDoubleSidedPipeline->SetDescriptorPool(descriptorPool->Get());
		transparentDoubleSidedPipeline->SetDescriptorPool(descriptorPool->Get());

		sync = std::make_unique<VulkanSync>(device->GetLogical());
	}

	Renderer::~Renderer()
	{

	}

	VulkanDevice* Renderer::GetDevice() const
	{
		return device.get();
	}

	VulkanInstance* Renderer::GetInstance() const
	{
		return instance.get();
	}

	VulkanSwapChain* Renderer::GetSwapChain() const
	{
		return swapChain.get();
	}

	VulkanRenderPass* Renderer::GetRenderPass() const
	{
		return renderPass.get();
	}

	VulkanDescriptorSetLayoutManager* Renderer::GetDescriptorSetLayoutManager() const
	{
		return descriptorSetLayoutManager.get();
	}

	GlobalDescriptorSetManager* Renderer::GetGlobalDescriptorSetManager() const
	{
		return globalDescriptorSetManager.get();
	}

	VulkanDescriptorPool* Renderer::GetDescriptorPool() const
	{
		return descriptorPool.get();
	}

	void Renderer::SetRenderTarget(RenderTarget* target)
	{
		renderTarget = target;
	}

	void Renderer::DrawFrame(Scene* scene)
	{
		vkWaitForFences(device->GetLogical(), 1, &sync->inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		VkCommandBuffer commandBuffer = device->commandBuffers[currentFrame];
		vkResetCommandBuffer(commandBuffer, 0);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(device->GetLogical(), swapChain->Get(), UINT64_MAX, sync->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
		{
			framebufferResized = false;
			RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS)
		{
			std::cerr << "Failed to acquire swap chain image" << std::endl;
			return;
		}

		renderPass->BeginCommandBuffer(commandBuffer);

		renderTarget->Render(scene, renderPass.get(), commandBuffer, swapChain->framebuffers[imageIndex], swapChain->extent);

		renderPass->EndCommandBuffer(commandBuffer);

		vkResetFences(device->GetLogical(), 1, &sync->inFlightFences[currentFrame]);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = {sync->imageAvailableSemaphores[currentFrame]};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &device->commandBuffers[currentFrame];

		VkSemaphore signalSemaphores[] = {sync->renderFinishedSemaphores[currentFrame]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(device->graphicsQueue, 1, &submitInfo, sync->inFlightFences[currentFrame]) != VK_SUCCESS)
		{
			std::cerr << "Failed to submit draw command buffer" << std::endl;
			return;
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = {swapChain->Get()};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		result = vkQueuePresentKHR(device->presentQueue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
		{
			framebufferResized = false;
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			std::cerr << "Failed to present swap chain image" << std::endl;
		}

		currentFrame = (currentFrame + 1) % VulkanConfig::MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::DrawScene(Scene* scene, VkCommandBuffer commandBuffer, VkExtent2D extent)
	{
		scene->UpdateBuffers(currentFrame, extent);

		Camera* mainCamera = scene->GetMainCamera();
		if (!mainCamera)
			return;

		std::vector<Renderable> opaqueRenderables;
		std::vector<Renderable> transparentRenderables;
		std::vector<Renderable> opaqueDoubleSidedRenderables;
		std::vector<Renderable> transparentDoubleSidedRenderables;

		for (const auto& object : scene->GetRootObject()->GetChildren())
		{
			CollectRenderables(object.get(), opaqueRenderables, transparentRenderables, opaqueDoubleSidedRenderables, transparentDoubleSidedRenderables);
		}

		glm::vec3 cameraWorldPos = glm::vec3(mainCamera->GetWorldMatrix()[3]);

		std::sort(transparentRenderables.begin(), transparentRenderables.end(),
			[&](const Renderable& a, const Renderable& b)
			{
				glm::vec3 posA = glm::vec3(a.instance->GetWorldMatrix()[3]);
				glm::vec3 posB = glm::vec3(b.instance->GetWorldMatrix()[3]);
				
				float distA = glm::length(cameraWorldPos - posA);
				float distB = glm::length(cameraWorldPos - posB);
				return distA > distB;
			});

		std::sort(transparentDoubleSidedRenderables.begin(), transparentDoubleSidedRenderables.end(),
			[&](const Renderable& a, const Renderable& b)
			{
				glm::vec3 posA = glm::vec3(a.instance->GetWorldMatrix()[3]);
				glm::vec3 posB = glm::vec3(b.instance->GetWorldMatrix()[3]);

				float distA = glm::length(cameraWorldPos - posA);
				float distB = glm::length(cameraWorldPos - posB);
				return distA > distB;
			});
		
		opaquePipeline->Render(commandBuffer, currentFrame, opaqueRenderables, mainCamera);
		transparentPipeline->Render(commandBuffer, currentFrame, transparentRenderables, mainCamera);
		opaqueDoubleSidedPipeline->Render(commandBuffer, currentFrame, opaqueDoubleSidedRenderables, mainCamera);
		transparentDoubleSidedPipeline->Render(commandBuffer, currentFrame, transparentDoubleSidedRenderables, mainCamera);
	}

	void Renderer::FramebufferResized()
	{
		framebufferResized = true;
	}

	void Renderer::RecreateSwapChain()
	{
		int width = 0, height = 0;
		glfwWindow->GetFramebufferSize(&width, &height);

		while (width == 0 || height == 0)
		{
			glfwWindow->GetFramebufferSize(&width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device->GetLogical());

		swapChain->CleanupSwapChain();
		sync->CleanupSyncObjects();

		swapChain->CreateSwapChain();
		swapChain->CreateDepthResources();
		swapChain->CreateFramebuffers(renderPass->Get());
		sync->CreateSyncObjects();
	}

	void Renderer::CollectRenderables(SceneObject* object, std::vector<Renderable>& opaque, std::vector<Renderable>& transparent, std::vector<Renderable>& opaqueDoubleSided, std::vector<Renderable>& transparentDoubleSided)
	{
		if (!object)
			return;

		if (auto* instance = dynamic_cast<MeshInstance*>(object))
		{
			auto mesh = instance->GetMesh();

			for (size_t i = 0; i < mesh->GetPrimitiveCount(); ++i)
			{
				MeshPrimitive* primitive = mesh->GetPrimitive(i);

				Renderable renderable{ instance, primitive };

				bool transparencyEnabled = primitive->GetTransparencyEnabled();
				bool doubleSided = primitive->GetDoubleSided();

				if (transparencyEnabled && doubleSided)
					transparentDoubleSided.push_back(renderable);
				else if (transparencyEnabled)
					transparent.push_back(renderable);
				else if (doubleSided)
					opaqueDoubleSided.push_back(renderable);
				else
					opaque.push_back(renderable);
			}
		}

		for (const auto& child : object->GetChildren())
			CollectRenderables(child.get(), opaque, transparent, opaqueDoubleSided, transparentDoubleSided);
	}
}