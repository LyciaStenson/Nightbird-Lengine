#include "Core/Engine.h"
#include "Core/PlatformFactory.h"
#include "Core/RendererFactory.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"

int main()
{
	Nightbird::Engine engine(Nightbird::CreatePlatform(), Nightbird::CreateRenderer());

	engine.Run();

	return 0;
}