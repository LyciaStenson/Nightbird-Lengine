#pragma once

#include <unordered_set>
#include <vector>
#include <memory>

namespace Rml
{
	class Context;
	class ElementDocument;
}

namespace Nightbird
{
	class GlfwWindow;
	class ModelManager;
	class MeshInstance;
	class Scene;
	class Renderer;
	
	class Engine
	{
	public:
		Engine();
		~Engine();
		
		GlfwWindow* GetGlfwWindow() const;
		Renderer* GetRenderer() const;
		Scene* GetScene() const;
		ModelManager* GetModelManager() const;

		float GetDeltaTime() const;
		
		void Run();

	private:
		std::unique_ptr<GlfwWindow> glfwWindow;

		std::unique_ptr<Renderer> renderer;
		
		std::unique_ptr<Scene> scene;
		std::unique_ptr<ModelManager> modelManager;

		Rml::Context* context = nullptr;
		Rml::ElementDocument* document = nullptr;

		float deltaTime = 0.0f;
	};
}