#include "Core/Engine.h"
#include "Core/BackendFactory.h"

int main()
{
	auto platform = Nightbird::Core::CreatePlatform();
	auto renderer = Nightbird::Core::CreateRenderer();

	Nightbird::Core::Engine engine(std::move(platform), std::move(renderer));

	engine.Initialize();
	engine.RunLoop();
	engine.Shutdown();

	return 0;
}
