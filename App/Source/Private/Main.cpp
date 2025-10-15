#include <Core/Engine.h>

#include <Core/Renderer.h>
#include <Core/Scene.h>

#include <AppRenderTarget.h>

#include <iostream>

#include <rttr/library.h>

using namespace Nightbird;

int main(int argc, char** argv)
{
	rttr::library project("Project");
	bool projectLoad = project.load();
	if (!projectLoad)
		std::cout << "Failed to load Project shared library via RTTR" << std::endl;

	Engine engine;
	
	AppRenderTarget renderTarget(engine.GetRenderer());
	engine.GetRenderer()->SetRenderTarget(&renderTarget);
	
	engine.Run();
}