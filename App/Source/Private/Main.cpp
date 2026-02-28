#include "Core/Engine.h"
#include "Core/PlatformFactory.h"
#include "Core/RendererFactory.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "Input/Provider.h"

int main()
{
	Nightbird::Core::Engine engine(Nightbird::Core::CreatePlatform(), Nightbird::Core::CreateRenderer());

	engine.Run();

	return 0;
}