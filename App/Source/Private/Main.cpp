#include <Core/Engine.h>

#include <Core/Renderer.h>
#include <Core/Scene.h>

#include <AppRenderTarget.h>

using namespace Nightbird;

int main(int argc, char** argv)
{
	Engine engine;
	
	AppRenderTarget renderTarget(engine.GetRenderer());
	engine.GetRenderer()->SetRenderTarget(&renderTarget);
	
	engine.Run();
}