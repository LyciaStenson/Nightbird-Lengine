#include "Core/Engine.h"

#include "Core/SceneObject.h"
#include "Core/Renderer.h"
#include "Core/ModelManager.h"
#include "Core/GlfwWindow.h"
#include "Vulkan/DescriptorPool.h"

#include "EditorRenderTarget.h"

#include <iostream>
#include <filesystem>

#include <rttr/library.h>

using namespace Nightbird;

int main(int argc, char** argv)
{
	Engine engine;

	EditorRenderTarget renderTarget(engine.GetRenderer(), engine.GetRenderer()->GetInstance(), engine.GetRenderer()->GetDevice(), engine.GetRenderer()->GetSwapChain(), engine.GetRenderer()->GetRenderPass(), engine.GetGlfwWindow()->Get(), engine.GetScene(), engine.GetModelManager(), &engine);
	engine.GetRenderer()->SetRenderTarget(&renderTarget);

	if (argc > 1)
	{
		std::string path = argv[1];
		engine.LoadProject(path);
		renderTarget.StartEditor();
	}
	else
	{
		renderTarget.StartProjectManager();
	}

	engine.Run();
}