#include <Core/Engine.h>

#include <iostream>
#include <algorithm>

#include <volk.h>

#include <Core/GlfwWindow.h>
#include <Core/ModelManager.h>
#include <Core/Scene.h>
#include <Core/RenderTarget.h>
#include <Core/Renderer.h>
#include <Vulkan/Device.h>
#include <Vulkan/DescriptorPool.h>
#include <Vulkan/DescriptorSetLayoutManager.h>
#include <Vulkan/GlobalDescriptorSetManager.h>
#include <Core/GlmRegistration.h>
#include <Input.h>

namespace Nightbird
{
	Engine::Engine()
	{
		GlmRegistration();

		if (volkInitialize() != VK_SUCCESS)
		{
			std::cerr << "Failed to initialize Volk" << std::endl;
		}
		
		glfwWindow = std::make_unique<GlfwWindow>();
		Input::Get().Init(glfwWindow->Get());
		
		renderer = std::make_unique<Renderer>(glfwWindow.get());
		glfwWindow->SetUserPointer(renderer.get());

		modelManager = std::make_unique<ModelManager>(renderer->GetDevice(), renderer->GetDescriptorSetLayoutManager()->GetMeshDescriptorSetLayout(), renderer->GetDescriptorSetLayoutManager()->GetMaterialDescriptorSetLayout(), renderer->GetDescriptorPool()->Get());
		
		scene = std::make_unique<Scene>(renderer->GetDevice(), modelManager.get(), renderer->GetGlobalDescriptorSetManager(), renderer->GetDescriptorPool()->Get());
	}

	Engine::~Engine()
	{

	}

	GlfwWindow* Engine::GetGlfwWindow() const
	{
		return glfwWindow.get();
	}

	Renderer* Engine::GetRenderer() const
	{
		return renderer.get();
	}

	Scene* Engine::GetScene() const
	{
		return scene.get();
	}

	ModelManager* Engine::GetModelManager() const
	{
		return modelManager.get();
	}

	void Engine::Run()
	{
		while (!glfwWindowShouldClose(glfwWindow->Get()))
		{
			glfwPollEvents();
			Input::Get().ProcessEvents();
			modelManager->ProcessUploadQueue();
			renderer->DrawFrame(scene.get());
		}
		vkDeviceWaitIdle(renderer->GetDevice()->GetLogical());
	}
}