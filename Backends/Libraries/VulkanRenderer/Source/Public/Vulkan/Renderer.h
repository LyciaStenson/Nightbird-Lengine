#pragma once

#include "Core/Renderer.h"

#include "Core/MeshPrimitive.h"
#include "Core/Material.h"
#include "Core/MeshInstance.h"
#include "Core/Texture.h"
#include "Core/Renderable.h"
#include "Core/DirectionalLight.h"
#include "Core/PointLight.h"

#include "Vulkan/Instance.h"
#include "Vulkan/Device.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/SwapChainSurface.h"
#include "Vulkan/RenderPass.h"
#include "Vulkan/Sync.h"
#include "Vulkan/DescriptorSetLayoutManager.h"
#include "Vulkan/GlobalDescriptorSetManager.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/Geometry.h"
#include "Vulkan/Material.h"
#include "Vulkan/TransformPool.h"
#include "Vulkan/FrameContext.h"

#include <volk.h>

#include <vector>
#include <memory>
#include <unordered_map>

namespace Nightbird::Core
{
	class Platform;
	class Scene;
	class Camera;
}

namespace Nightbird::Vulkan
{
	using SurfaceCreator = VkSurfaceKHR(*)(VkInstance);

	class Renderer : public Core::Renderer
	{
	public:
		Renderer(std::vector<const char*> extensions, SurfaceCreator surfaceCreator);

		void Initialize() override;
		void Shutdown() override;
		Core::RenderSurface& GetDefaultSurface() override;
		void SubmitScene(const Core::Scene& scene, const Core::Camera& camera) override;
		void BeginFrame(Core::RenderSurface& surface) override;
		void EndFrame(Core::RenderSurface& surface) override;
		void DrawScene(Core::RenderSurface& surface) override;

		Instance& GetInstance();
		Device& GetDevice();
		SwapChain& GetSwapChain();
		RenderPass& GetRenderPass();

		VkCommandBuffer GetCurrentCommandBuffer() const;

	private:
		std::vector<const char*> m_Extensions;
		SurfaceCreator m_SurfaceCreator;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		std::unique_ptr<Instance> m_Instance;
		std::unique_ptr<Device> m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;
		std::unique_ptr<SwapChainSurface> m_SwapChainSurface;
		std::unique_ptr<RenderPass> m_RenderPass;
		std::unique_ptr<Sync> m_Sync;

		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

		std::unique_ptr<DescriptorSetLayoutManager> m_DescriptorSetLayoutManager;
		std::unique_ptr<GlobalDescriptorSetManager> m_GlobalDescriptorSetManager;
		std::unique_ptr<Pipeline> m_OpaquePipeline;
		std::unique_ptr<Pipeline> m_TransparentPipeline;

		std::unique_ptr<TransformPool> m_TransformPool;

		std::unordered_map<const Core::MeshPrimitive*, Geometry> m_GeometryCache;
		std::unordered_map<const Core::Material*, Material> m_MaterialCache;

		const Core::Camera* m_ActiveCamera = nullptr;

		std::vector<Core::Renderable> m_Renderables;
		std::vector<Core::DirectionalLight*> m_DirectionalLights;
		std::vector<Core::PointLight*> m_PointLights;

		FrameContext m_CurrentFrame;

		std::shared_ptr<Core::Texture> m_DefaultTexture;

		void DrawScene(VkCommandBuffer commandBuffer);
		void DrawRenderable(VkCommandBuffer commandBuffer, const Core::Renderable&, Pipeline* currentPipeline);

		void CreateDescriptorPool();

		std::shared_ptr<Core::Texture> CreateDefaultTexture();

		Geometry& GetOrCreateGeometry(const Core::MeshPrimitive* primitive);
		Material& GetOrCreateMaterial(const Core::Material* material);
	};
}
