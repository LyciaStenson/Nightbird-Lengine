#include "Core/Engine.h"
#include "Core/BackendFactory.h"
#include "Core/Log.h"

#include <rttr/library.h>

#include <string>

int main(int argc, char** argv)
{
	auto runEngine = []()
	{
		auto platform = Nightbird::Core::CreatePlatform();
		auto renderer = Nightbird::Core::CreateRenderer();

		Nightbird::Core::Engine engine(std::move(platform), std::move(renderer));
		engine.Initialize();
		engine.RunLoop();
		engine.Shutdown();
	};

	if (argc > 1)
	{
		rttr::library project(argv[1]);
		if (project.load())
		{
			Nightbird::Core::Log::Info("Loaded project: " + std::string(argv[1]));

			for (auto& type : rttr::type::get_types())
				Nightbird::Core::Log::Info("Registered type: " + type.get_name().to_string());

			runEngine();
		}
		else
		{
			Nightbird::Core::Log::Error("Failed to load project: " + std::string(argv[1]));
			Nightbird::Core::Log::Error(project.get_error_string().to_string());
		}
	}
	else
	{
		Nightbird::Core::Log::Info("No project specified");
		runEngine();
	}

	return 0;
}
