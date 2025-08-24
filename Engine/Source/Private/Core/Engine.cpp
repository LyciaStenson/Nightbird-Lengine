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
#include <Vulkan/SwapChain.h>
#include <Vulkan/Device.h>
#include <Vulkan/DescriptorPool.h>
#include <Vulkan/DescriptorSetLayoutManager.h>
#include <Vulkan/GlobalDescriptorSetManager.h>
#include <Core/GlmRegistration.h>
#include <Input.h>

#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>

#include <UI/SystemInterface.h>
#include <UI/RenderInterface.h>

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
		
		uiSystemInterface = std::make_unique<UISystemInterface>(glfwWindow->Get());

		uiRenderInterface = std::make_unique<UIRenderInterface>();
		//uiRenderInterface->Initialize(renderer->GetInstance()->Get(), renderer->GetDevice()->GetPhysical(), renderer->GetDevice()->GetPhysicalDeviceProperties(), renderer->GetDevice()->GetLogical(), renderer->GetDevice()->GetAllocator(), renderer->GetRenderPass()->Get(), renderer->GetInstance()->GetSurface(), renderer->GetDevice()->graphicsQueue, renderer->GetDevice()->presentQueue, renderer->GetDevice()->graphicsQueueFamily, renderer->GetDevice()->presentQueueFamily);
		
		int width = 1280, height = 720;
		glfwWindow->GetFramebufferSize(&width, &height);
		//uiRenderInterface->SetViewport(renderer->GetSwapChain()->Get(), width, height);
		
		Rml::SetSystemInterface(uiSystemInterface.get());
		Rml::SetRenderInterface(uiRenderInterface.get());
		
		Rml::Initialise();
		
		glfwGetWindowSize(glfwWindow->Get(), &width, &height);
		
		context = Rml::CreateContext("main", Rml::Vector2i(width, height));
		if (!context)
		{
			std::cerr << "Failed to create RmlUi Context" << std::endl;
			Rml::Shutdown();
		}

		Rml::Debugger::Initialise(context);

		Rml::LoadFontFace("Assets/Fonts/RobotoFlex-Regular.ttf");
		
		document = context->LoadDocument("Assets/Test.rml");
		if (!document)
		{
			std::cerr << "Failed to load RmlUi Test Document" << std::endl;
			Rml::Shutdown();
		}
		document->Show();
	}
	
	Engine::~Engine()
	{
		document->Close();
		Rml::Shutdown();
		//uiRenderInterface->Shutdown();
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

			if (context)
			{
				Rml::Vector2 dimensions = context->GetDimensions();
				if (dimensions.x != 1280 || dimensions.y != 720)
					std::cout << dimensions.x << ", " << dimensions.y << std::endl;

				context->Update();

				dimensions = context->GetDimensions();
				if (dimensions.x != 1280 || dimensions.y != 720)
					std::cout << dimensions.x << ", " << dimensions.y << std::endl;
			}
			
			renderer->DrawFrame(scene.get(), uiRenderInterface.get(), context);
		}
		vkDeviceWaitIdle(renderer->GetDevice()->GetLogical());
	}
}