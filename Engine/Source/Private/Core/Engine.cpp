#include "Core/Engine.h"

#include "Core/Platform.h"
#include "Core/Renderer.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"

#include "Core/Log.h"

#include "Input/Provider.h"

#include <chrono>

namespace Nightbird::Core
{
	Engine::Engine(std::unique_ptr<Platform> platform, std::unique_ptr<Renderer> renderer)
		: m_Platform(std::move(platform)), m_Renderer(std::move(renderer))
	{
		m_Scene = std::make_unique<Scene>();
	}
	
	void Engine::Run()
	{
		m_Platform->Initialize();
		m_Renderer->Initialize();

		MainLoop();

		m_Renderer->Shutdown();
		m_Platform->Shutdown();
	}

	void Engine::MainLoop()
	{
		auto lastTime = std::chrono::high_resolution_clock::now();
		
		while (!m_Platform->ShouldClose())
		{
			auto currentTime = std::chrono::high_resolution_clock::now();
			float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
			lastTime = currentTime;

			m_Platform->PollEvents();

			m_InputSystem.Update(m_Platform->GetInputProvider());
			
			m_Scene->Update(deltaTime);
			
			if (m_InputSystem.WasButtonPressed(Input::Button::A))
				Log::Info("A Pressed");

			m_Renderer->SubmitScene(*m_Scene);

			m_Renderer->DrawFrame();
		}
	}

	Input::System& Engine::GetInputSystem()
	{
		return m_InputSystem;
	}

	Scene& Engine::GetScene()
	{
		return *m_Scene;
	}
}