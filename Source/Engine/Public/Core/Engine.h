#pragma once

#include <unordered_set>
#include <string>
#include <vector>
#include <memory>

#include <volk.h>

#include <GLFW/glfw3.h>

namespace Nightbird
{
	class GlfwWindow;
	class ModelManager;
	class MeshInstance;
	class SceneManager;
	class Renderer;
	
	class Engine
	{
	public:
		Engine();
		~Engine();
		
		GlfwWindow* GetGlfwWindow() const;
		Renderer* GetRenderer() const;
		SceneManager* GetScene() const;
		ModelManager* GetModelManager() const;

		float GetDeltaTime() const;
		
		void Run();

		void LoadProject(const std::string& path);

		bool bSimulationRunning = false;

	private:
		std::unique_ptr<GlfwWindow> glfwWindow;

		std::unique_ptr<Renderer> renderer;
		
		std::unique_ptr<SceneManager> scene;
		std::unique_ptr<ModelManager> modelManager;

		float deltaTime = 0.0f;
	};
}