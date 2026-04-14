#include "Core/Engine.h"

#include "Core/Platform.h"
#include "Core/Renderer.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"

#include "Core/Log.h"

#include "Input/InputProvider.h"

#include <chrono>

#ifndef EDITOR_BUILD
namespace Nightbird
{
	void ForceRegistrationLink();
}
#endif

namespace Nightbird::Core
{
	Engine::Engine(std::unique_ptr<Platform> platform, std::unique_ptr<Renderer> renderer)
		: m_Platform(std::move(platform)), m_Renderer(std::move(renderer))
	{
#ifndef EDITOR_BUILD
		ForceRegistrationLink();
#endif

		m_Scene = std::make_unique<Scene>();

		m_Platform->Initialize();
		m_Renderer->Initialize();

		if (m_Scene)
			m_Scene->SetEngine(this);
	}

	Engine::~Engine()
	{
		m_Renderer->Shutdown();
		m_Platform->Shutdown();
	}

	bool Engine::ShouldClose() const
	{
		return m_Platform->ShouldClose();
	}

	float Engine::Update()
	{
		static auto lastTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
		lastTime = currentTime;

		m_Platform->Update();
		m_InputSystem.Update(m_Platform->GetInputProvider());
		m_Scene->Update(deltaTime);

		if (m_InputSystem.WasButtonPressed(Input::Button::A))
			Log::Info("A Pressed");

		return deltaTime;
	}

	Platform& Engine::GetPlatform()
	{
		return *m_Platform;
	}

	Renderer& Engine::GetRenderer()
	{
		return *m_Renderer;
	}

	Input::System& Engine::GetInputSystem()
	{
		return m_InputSystem;
	}

	Audio::Provider& Engine::GetAudioProvider()
	{
		return m_Platform->GetAudioProvider();
	}

	Scene& Engine::GetScene()
	{
		return *m_Scene;
	}

	void Engine::SetScene(std::unique_ptr<Scene> scene)
	{
		m_Scene = std::move(scene);
		m_Scene->SetEngine(this);
	}
}
