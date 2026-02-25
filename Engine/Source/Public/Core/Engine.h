#pragma once

#include <memory>

namespace Nightbird
{
	class Platform;
	class Renderer;

	class Engine
	{
	public:
		Engine(std::unique_ptr<Platform> platform, std::unique_ptr<Renderer> renderer);

		void Run();

	private:
		void MainLoop();

		std::unique_ptr<Platform> m_Platform;
		std::unique_ptr<Renderer> m_Renderer;
	};
}