#include "PICA/Renderer.h"

#include "Core/Scene.h"
#include "Core/Camera.h"
#include "Core/Log.h"

#include <3ds.h>

#include "Shader_v_pica_shbin.h"

#define CLEAR_COLOR 0x000000FF
#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

typedef struct
{
	float x, y, z;
} vertex;

static const vertex triangleVertices[] =
{
	{  0.0f,  0.5f, 0.5f },
	{ -0.5f, -0.5f, 0.5f },
	{  0.5f, -0.5f, 0.5f }
};

namespace Nightbird::PICA
{
	void Renderer::Initialize()
	{
		C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

		m_TopTarget = C3D_RenderTargetCreate(240, 480, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
		C3D_RenderTargetSetOutput(m_TopTarget, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

		m_ShaderDvlb = DVLB_ParseFile((u32*)Shader_v_pica_shbin, Shader_v_pica_shbin_size);
		shaderProgramInit(&m_ShaderProgram);
		shaderProgramSetVsh(&m_ShaderProgram, &m_ShaderDvlb->DVLE[0]);
		C3D_BindProgram(&m_ShaderProgram);

		m_ULocProjection = shaderInstanceGetUniformLocation(m_ShaderProgram.vertexShader, "projection");
		m_ULocModelView = shaderInstanceGetUniformLocation(m_ShaderProgram.vertexShader, "modelView");

		C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
		AttrInfo_Init(attrInfo);
		AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); //v0 position
		AttrInfo_AddFixed(attrInfo, 1); // v1 color
		C3D_FixedAttribSet(1, 1.0f, 1.0f, 1.0f, 1.0f);

		m_VertexBuffer = linearAlloc(sizeof(triangleVertices));
		memcpy(m_VertexBuffer, triangleVertices, sizeof(triangleVertices));

		C3D_BufInfo* bufInfo = C3D_GetBufInfo();
		BufInfo_Init(bufInfo);
		BufInfo_Add(bufInfo, m_VertexBuffer, sizeof(vertex), 1, 0x0);

		C3D_TexEnv* env = C3D_GetTexEnv(0);
		C3D_TexEnvInit(env);
		C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
		C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);

		Core::Log::Info("Renderer initialized");
	}

	void Renderer::Shutdown()
	{
		linearFree(m_VertexBuffer);
		shaderProgramFree(&m_ShaderProgram);
		DVLB_Free(m_ShaderDvlb);
		C3D_RenderTargetDelete(m_TopTarget);
		C3D_Fini();
	}

	void Renderer::SubmitScene(const Core::Scene& scene)
	{
		m_ActiveCamera = scene.GetActiveCamera();
		m_Renderables = scene.CollectRenderables();
	}

	void Renderer::DrawFrame()
	{
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C3D_RenderTargetClear(m_TopTarget, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
		C3D_FrameDrawOn(m_TopTarget);

		DrawScene();

		C3D_FrameEnd(0);
	}

	void Renderer::DrawScene()
	{
		C3D_Mtx projection;
		Mtx_PerspTilt(&projection, C3D_AngleFromDegrees(60.0f), C3D_AspectRatioTop, 0.01f, 1000.0f, false);

		C3D_Mtx modelView;
		Mtx_Identity(&modelView);
		Mtx_Translate(&modelView, 0.0f, 0.0f, -2.0f, true);

		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, m_ULocProjection, &projection);
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, m_ULocModelView, &modelView);

		C3D_DrawArrays(GPU_TRIANGLES, 0, 3);
	}
}
