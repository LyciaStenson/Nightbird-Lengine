#pragma once

#include <memory>

#include "Input/System.h"

namespace Nightbird::Core
{
	class Platform;
	class Renderer;
	class Scene;

	class Engine
	{
	public:
		Engine(std::unique_ptr<Platform> platform, std::unique_ptr<Renderer> renderer);

		void Run();

		Input::System& GetInputSystem();

		Scene& GetScene();

	private:
		void MainLoop();

		std::unique_ptr<Platform> m_Platform;
		std::unique_ptr<Renderer> m_Renderer;

		std::unique_ptr<Scene> m_Scene;

		Input::System m_InputSystem;
	};
}