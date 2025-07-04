#include <Core/Engine.h>

#include <Core/Renderer.h>
#include <Core/ModelManager.h>
#include <Core/Scene.h>
#include <Core/SceneObject.h>
#include <Core/Camera.h>
#include <Core/GlfwWindow.h>
#include <Vulkan/DescriptorPool.h>

#include <EditorRenderTarget.h>

using namespace Nightbird;

int main(int argc, char** argv)
{
	Engine engine;
	
	EditorRenderTarget renderTarget(engine.GetRenderer(), engine.GetRenderer()->GetInstance(), engine.GetRenderer()->GetDevice(), engine.GetRenderer()->GetSwapChain(), engine.GetRenderer()->GetRenderPass(), engine.GetGlfwWindow()->Get(), engine.GetScene(), engine.GetModelManager());
	engine.GetRenderer()->SetRenderTarget(&renderTarget);
	
	engine.Run();
}