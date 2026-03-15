#include "PICA/Renderer.h"

#include "Core/Scene.h"
#include "Core/Log.h"

#define CLEAR_COLOR 0x000000FF
#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

namespace Nightbird::PICA
{
	void Renderer::Initialize()
	{
		C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
		m_TopTarget = C3D_RenderTargetCreate(240, 480, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
		C3D_RenderTargetSetOutput(m_TopTarget, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

		Core::Log::Info("Renderer initialized");
	}

	void Renderer::Shutdown()
	{
		C3D_RenderTargetDelete(m_TopTarget);
		C3D_Fini();
	}

	void Renderer::SubmitScene(const Core::Scene& scene)
	{
		m_Renderables = scene.CollectRenderables();
	}

	void Renderer::DrawFrame()
	{
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C3D_RenderTargetClear(m_TopTarget, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
		C3D_FrameDrawOn(m_TopTarget);
		C3D_FrameEnd(0);
	}
}
