#include "Core/Engine.h"
#include "Core/PlatformFactory.h"
#include "Core/RendererFactory.h"

int main()
{
	Nightbird::Engine engine(Nightbird::CreatePlatform(), Nightbird::CreateRenderer());

	engine.Run();

	return 0;
}