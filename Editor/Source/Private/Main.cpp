#include <Core/Engine.h>

#include <Core/SceneObject.h>
#include <Core/SceneObjectRegistry.h>
#include <Core/Renderer.h>
#include <Core/ModelManager.h>
#include <Core/GlfwWindow.h>
#include <Vulkan/DescriptorPool.h>

#include <Core/ProjectExport.h>

#include <EditorRenderTarget.h>

#include <iostream>

#include <rttr/library.h>

using namespace Nightbird;

int main(int argc, char** argv)
{
	rttr::library project("Project");
	bool projectLoaded = project.load();
	if (projectLoaded)
	{
		std::cout << "Loaded Project shared library via RTTR" << std::endl;
		for (auto& type : project.get_types())
		{
			std::cout << type.get_name() << std::endl;
		}
	}
	else
	{
		std::cout << "Failed to load Project shared library via RTTR" << std::endl;
	}
	
	for (auto& type : rttr::type::get_types())
	{
		std::cout << "GLOBAL: " << type.get_name() << std::endl;
	}

	Engine engine;
	
	EditorRenderTarget renderTarget(engine.GetRenderer(), engine.GetRenderer()->GetInstance(), engine.GetRenderer()->GetDevice(), engine.GetRenderer()->GetSwapChain(), engine.GetRenderer()->GetRenderPass(), engine.GetGlfwWindow()->Get(), engine.GetScene(), engine.GetModelManager());
	engine.GetRenderer()->SetRenderTarget(&renderTarget);
	
	engine.Run();
}