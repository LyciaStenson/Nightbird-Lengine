#pragma once

#include <memory>

#include "Input/InputSystem.h"
#include "Audio/AudioProvider.h"
#include "Core/AudioAsset.h"

namespace Nightbird::Core
{
	class Platform;
	class Renderer;
	class Scene;

	class Engine
	{
	public:
		Engine(std::unique_ptr<Platform> platform, std::unique_ptr<Renderer> renderer);
		~Engine();

		void Initialize();
		void RunLoop();
		void Shutdown();

		Input::System& GetInputSystem();
		Audio::Provider& GetAudioProvider();

		Scene& GetScene();
		void SetScene(std::unique_ptr<Scene> scene);

	private:
		void MainLoop();

		std::unique_ptr<Platform> m_Platform;
		std::unique_ptr<Renderer> m_Renderer;

		std::unique_ptr<Scene> m_Scene;

		Input::System m_InputSystem;
	};
}
