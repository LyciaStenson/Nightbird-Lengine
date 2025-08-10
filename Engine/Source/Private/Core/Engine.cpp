#include <Core/Engine.h>

#include <iostream>
#include <algorithm>

#include <volk.h>

#include <Core/GlfwWindow.h>
#include <Core/ModelManager.h>
#include <Core/Scene.h>
#include <Core/RenderTarget.h>
#include <Core/Renderer.h>
#include <Vulkan/Instance.h>
#include <Vulkan/Device.h>
#include <Vulkan/DescriptorPool.h>
#include <Vulkan/DescriptorSetLayoutManager.h>
#include <Vulkan/GlobalDescriptorSetManager.h>
#include <Core/GlmRegistration.h>
#include <Input.h>

#include <RmlUi/Core.h>
#include <RmlUi/Backends/RmlUi_Backend.h>

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
		
		int width, height;
		glfwWindow->GetFramebufferSize(&width, &height);
		if (!Backend::Initialize(renderer->GetInstance()->Get(), renderer->GetDevice()->GetLogical(), renderer->GetDevice()->GetPhysical(), renderer->GetDevice()->GetPhysicalDeviceProperties(), renderer->GetInstance()->GetSurface(), renderer->GetDevice()->graphicsQueueFamily, renderer->GetDevice()->graphicsQueue, renderer->GetDevice()->presentQueueFamily, renderer->GetDevice()->presentQueue, glfwWindow->Get()))
		{
			std::cerr << "Failed to inialize backend" << std::endl;
		}

		Rml::SetSystemInterface(Backend::GetSystemInterface());
		Rml::SetRenderInterface(Backend::GetRenderInterface());

		Rml::Initialise();

		//context = Rml::CreateContext("main", Rml::Vector2i(width, height));
		//if (!context)
		//{
			//std::cerr << "Failed to create context" << std::endl;
		//}
	}
	
	Engine::~Engine()
	{
		Rml::Shutdown();
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

	float Engine::GetDeltaTime() const
	{
		return deltaTime;
	}

	void Engine::Run()
	{
		double lastTime = glfwGetTime();

		while (!glfwWindowShouldClose(glfwWindow->Get()))
		{
			glfwPollEvents();
			Input::Get().ProcessEvents();

			double currentTime = glfwGetTime();
			deltaTime = static_cast<float>(currentTime - lastTime);
			lastTime = currentTime;

			scene->Update(deltaTime);

			modelManager->ProcessUploadQueue();
			renderer->DrawFrame(scene.get());
		}
		vkDeviceWaitIdle(renderer->GetDevice()->GetLogical());
	}
}