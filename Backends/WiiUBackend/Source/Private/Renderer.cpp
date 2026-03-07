#include "Renderer.h"

#include "Core/Scene.h"

namespace Nightbird::WiiU
{
	void Renderer::Initialize()
	{

	}

	void Renderer::Shutdown()
	{

	}

	void Renderer::SubmitScene(const Core::Scene& scene)
	{
		m_Renderables = scene.CollectRenderables();
	}

	void Renderer::DrawFrame()
	{

	}
}