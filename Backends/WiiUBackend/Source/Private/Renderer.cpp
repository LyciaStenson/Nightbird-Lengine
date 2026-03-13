#include "Renderer.h"

#include "Core/Scene.h"
#include "Core/Log.h"

#include <gfd.h>
#include <gx2/draw.h>
#include <gx2r/draw.h>

#include <cstring>

#include "Shader_gsh.h"

namespace Nightbird::WiiU
{
	static const float sPositionData[] =
	{
		 0.0f,  0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f, 1.0f
	};

	void Renderer::Initialize()
	{
		WHBGfxInit();

		WHBGfxLoadGFDShaderGroup(&m_ShaderGroup, 0, Shader_gsh);
		WHBGfxInitShaderAttribute(&m_ShaderGroup, "aPosition", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
		WHBGfxInitFetchShader(&m_ShaderGroup);

		m_PositionBuffer.flags = GX2R_RESOURCE_BIND_VERTEX_BUFFER |
								 GX2R_RESOURCE_USAGE_CPU_READ |
								 GX2R_RESOURCE_USAGE_CPU_WRITE |
								 GX2R_RESOURCE_USAGE_GPU_READ;

		m_PositionBuffer.elemSize = 4 * sizeof(float);
		m_PositionBuffer.elemCount = 3;
		GX2RCreateBuffer(&m_PositionBuffer);

		void* buffer = GX2RLockBufferEx(&m_PositionBuffer, GX2R_RESOURCE_BIND_NONE);
		memcpy(buffer, sPositionData, m_PositionBuffer.elemSize * m_PositionBuffer.elemCount);
		GX2RUnlockBufferEx(&m_PositionBuffer, GX2R_RESOURCE_BIND_NONE);

		Core::Log::Info("WiiU Renderer Initialized");
	}

	void Renderer::Shutdown()
	{
		GX2RDestroyBufferEx(&m_PositionBuffer, GX2R_RESOURCE_BIND_NONE);
		WHBGfxFreeShaderGroup(&m_ShaderGroup);
		WHBGfxShutdown();
	}

	void Renderer::SubmitScene(const Core::Scene& scene)
	{
		m_Renderables = scene.CollectRenderables();
	}

	void Renderer::DrawFrame()
	{
		WHBGfxBeginRender();

		WHBGfxBeginRenderTV();
		WHBGfxClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		DrawScene();
		WHBGfxFinishRenderTV();

		WHBGfxBeginRenderDRC();
		WHBGfxClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		DrawScene();
		WHBGfxFinishRenderDRC();

		WHBGfxFinishRender();
	}

	void Renderer::DrawScene()
	{
		GX2SetFetchShader(&m_ShaderGroup.fetchShader);
		GX2SetVertexShader(m_ShaderGroup.vertexShader);
		GX2SetPixelShader(m_ShaderGroup.pixelShader);
		GX2RSetAttributeBuffer(&m_PositionBuffer, 0, m_PositionBuffer.elemSize, 0);
		GX2DrawEx(GX2_PRIMITIVE_MODE_TRIANGLES, 3, 0, 1);
	}
}
