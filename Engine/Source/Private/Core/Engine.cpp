#include "Core/Engine.h"

#include "Core/Platform.h"
#include "Core/Renderer.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"

namespace Nightbird
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
		while (!m_Platform->ShouldClose())
		{
			m_Platform->PollEvents();

			m_Scene->Update(0.001f); // Replace with delta calculation

			m_Renderer->DrawFrame();
		}
	}
}