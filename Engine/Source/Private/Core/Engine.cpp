#include "Core/Engine.h"

#include "Core/Platform.h"
#include "Core/Renderer.h"

namespace Nightbird
{
	Engine::Engine(Platform* platform, Renderer* renderer)
		: m_Platform(platform), m_Renderer(renderer)
	{

	}

	Engine::~Engine()
	{
		delete m_Platform;
		m_Platform = nullptr;

		delete m_Renderer;
		m_Renderer = nullptr;
	}

	void Engine::Run()
	{
		m_Platform->Initialize();
		m_Renderer->Initialize();
	}

	void Engine::MainLoop()
	{
		while (!m_Platform->ShouldClose())
		{
			m_Platform->PollEvents();
			m_Renderer->DrawFrame();
		}
	}
}