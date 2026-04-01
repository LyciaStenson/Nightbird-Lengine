#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Core/Camera.h"
#include "Core/Platform.h"
#include "Core/Renderer.h"
#include "Core/BackendFactory.h"

int main()
{
	auto platform = Nightbird::Core::CreatePlatform();
	auto renderer = Nightbird::Core::CreateRenderer();
	Nightbird::Core::Engine engine(std::move(platform), std::move(renderer));

	while (!engine.ShouldClose())
	{
		engine.Update();
		auto& surface = engine.GetRenderer().GetDefaultSurface();
		engine.GetRenderer().BeginFrame(surface);
		if (engine.GetScene().GetActiveCamera())
			engine.GetRenderer().SubmitScene(engine.GetScene(), *engine.GetScene().GetActiveCamera());
		engine.GetRenderer().DrawScene(surface);
		engine.GetRenderer().EndFrame(surface);
	}

	return 0;
}
