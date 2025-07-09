#include <Core/Engine.h>

#include <Core/SceneObject.h>
#include <Core/SceneObjectRegistry.h>
#include <Core/Renderer.h>
#include <Core/ModelManager.h>
#include <Core/GlfwWindow.h>
#include <Vulkan/DescriptorPool.h>

#include <Core/ProjectExport.h>

#include <EditorRenderTarget.h>

#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif

using namespace Nightbird;

int main(int argc, char** argv)
{
#ifdef _WIN32
	HMODULE project = LoadLibraryA("Project.dll");
	if (!project)
	{
		std::cout << "Failed to load Project.dll" << std::endl;
	}
	
	using GetSceneObjectCountFunc = int (*)();
	using GetSceneObjectDescriptorFunc = const SceneObjectDescriptor* (*)(int);

	GetSceneObjectCountFunc getCount = nullptr;
	GetSceneObjectDescriptorFunc getDescriptor = nullptr;
	
	if (project)
	{
		getCount = (GetSceneObjectCountFunc)GetProcAddress(project, "GetSceneObjectCount");
		getDescriptor = (GetSceneObjectDescriptorFunc)GetProcAddress(project, "GetSceneObjectDescriptor");
		g_DeleteCustomObject = (DeleteCustomObjectFunc)GetProcAddress(project, "DeleteCustomObject");

		if (!getCount || !getDescriptor)
			std::cerr << "Failed to load Project DLL exports" << std::endl;
		
		if (!g_DeleteCustomObject)
			std::cerr << "Failed to load DeleteCustomObject from Project DLL" << std::endl;
	}
#endif
	
	{
		Engine engine;

		EditorRenderTarget renderTarget(engine.GetRenderer(), engine.GetRenderer()->GetInstance(), engine.GetRenderer()->GetDevice(), engine.GetRenderer()->GetSwapChain(), engine.GetRenderer()->GetRenderPass(), engine.GetGlfwWindow()->Get(), engine.GetScene(), engine.GetModelManager());
		engine.GetRenderer()->SetRenderTarget(&renderTarget);

#ifdef _WIN32
		if (getCount && getDescriptor)
		{
			std::vector<const SceneObjectDescriptor*> objectDescriptors;

			int count = getCount();
			for (int i = 0; i < count; ++i)
			{
				const SceneObjectDescriptor* objectDescriptor = getDescriptor(i);
				if (objectDescriptor)
				{
					GetSceneObjectRegistry().push_back(*objectDescriptor);
					objectDescriptors.push_back(objectDescriptor);
				}
			}
			renderTarget.SetObjectTypes(objectDescriptors);
		}
#endif

		engine.Run();
	}

#ifdef _WIN32
	if (project)
		FreeLibrary(project);
#endif
}