#include "Core/Engine.h"
#include "Core/BackendFactory.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "Input/Provider.h"

int main()
{
	auto platform = Nightbird::Core::CreatePlatform();
	auto renderer = Nightbird::Core::CreateRenderer();

	Nightbird::Core::Engine engine(std::move(platform), std::move(renderer));

	engine.Run();

	return 0;
}