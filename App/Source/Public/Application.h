#pragma once

#include "Core/Engine.h"
#include "Core/Platform.h"
#include "Core/Renderer.h"
#include "Core/AssetLoader.h"
#include "Core/AssetManager.h"

#include <memory>

namespace Nightbird::App
{
	class Application
	{
	public:
		Application() = default;
		~Application() = default;

		int Run();

	private:
		void Initialize();
		int LoadProject();
		void RunLoop();
		void Shutdown();
		
		std::unique_ptr<Core::Platform> m_Platform;
		std::unique_ptr<Core::Renderer> m_Renderer;
		std::unique_ptr<Core::AssetLoader> m_AssetLoader;
		std::unique_ptr<Core::AssetManager> m_AssetManager;
		std::unique_ptr<Core::Engine> m_Engine;
	};
}