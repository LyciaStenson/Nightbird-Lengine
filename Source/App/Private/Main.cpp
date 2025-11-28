#include "Core/Engine.h"

#include "Core/Renderer.h"
#include "Core/SceneManager.h"

#include "AppRenderTarget.h"

#include <iostream>
#include <filesystem>

#include <rttr/library.h>

using namespace Nightbird;

int main(int argc, char** argv)
{
	if (argc > 1)
	{
		rttr::library project(argv[1]);
		bool projectLoad = project.load();
		if (projectLoad)
			std::cout << "Loaded project shared library" << std::endl;
		else
			std::cout << "Failed to load project shared library" << std::endl;
	}
	else
	{
		std::cout << "No project argument found" << std::endl;
	}

	Engine engine;
	
	AppRenderTarget renderTarget(engine.GetRenderer());
	engine.GetRenderer()->SetRenderTarget(&renderTarget);
	
	engine.bSimulationRunning = true;

	engine.Run();
}