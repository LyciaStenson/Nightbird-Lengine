#include "Core/Engine.h"

#include "Core/SceneObject.h"
#include "Core/Renderer.h"
#include "Core/ModelManager.h"
#include "Core/GlfwWindow.h"
#include "Vulkan/DescriptorPool.h"

#include "EditorRenderTarget.h"

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
	
	EditorRenderTarget renderTarget(engine.GetRenderer(), engine.GetRenderer()->GetInstance(), engine.GetRenderer()->GetDevice(), engine.GetRenderer()->GetSwapChain(), engine.GetRenderer()->GetRenderPass(), engine.GetGlfwWindow()->Get(), engine.GetScene(), engine.GetModelManager(), &engine);
	engine.GetRenderer()->SetRenderTarget(&renderTarget);
	
	engine.Run();
}