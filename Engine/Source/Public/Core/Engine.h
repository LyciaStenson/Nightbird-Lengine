#pragma once

namespace Nightbird
{
	class Engine
	{
	public:
		Engine(class Platform* platform, class Renderer* renderer);
		~Engine();

		void Run();

	private:
		void MainLoop();

		Platform* m_Platform;
		Renderer* m_Renderer;
	};
}