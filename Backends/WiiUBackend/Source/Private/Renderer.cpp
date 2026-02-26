#include "Renderer.h"

#include <whb/log_console.h>

namespace Nightbird::WiiU
{
	void Renderer::Initialize()
	{

	}

	void Renderer::Shutdown()
	{

	}

	void Renderer::DrawFrame()
	{
		WHBLogConsoleDraw();
	}
}