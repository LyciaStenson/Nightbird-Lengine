#include "PICA/PICARenderer.h"

#include "Core/Scene.h"
#include "Core/Camera.h"
#include "Core/MeshPrimitive.h"
#include "Core/Material.h"
#include "Core/Texture.h"
#include "Core/Log.h"

#include <3ds.h>

#include "Shader_v_pica_shbin.h"

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

		m_TopTarget = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
		C3D_RenderTargetSetOutput(m_TopTarget, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

		m_ShaderDvlb = DVLB_ParseFile((u32*)Shader_v_pica_shbin, Shader_v_pica_shbin_size);
		shaderProgramInit(&m_ShaderProgram);
		shaderProgramSetVsh(&m_ShaderProgram, &m_ShaderDvlb->DVLE[0]);
		C3D_BindProgram(&m_ShaderProgram);

		m_ULocProjection = shaderInstanceGetUniformLocation(m_ShaderProgram.vertexShader, "projection");
		m_ULocModelView = shaderInstanceGetUniformLocation(m_ShaderProgram.vertexShader, "modelView");

		C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
		AttrInfo_Init(attrInfo);
		AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0 position
		AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1 texcoord

		C3D_TexEnv* env = C3D_GetTexEnv(0);
		C3D_TexEnvInit(env);
		C3D_TexEnvSrc(env, C3D_RGB, GPU_TEXTURE0, GPU_CONSTANT, GPU_CONSTANT);
		C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE); // Multiply vertex and texture color

		C3D_TexEnvSrc(env, C3D_Alpha, GPU_TEXTURE0, GPU_CONSTANT, GPU_CONSTANT);
		C3D_TexEnvFunc(env, C3D_Alpha, GPU_REPLACE); // Multiply vertex and texture color

		C3D_TexEnvColor(env, 0xFFFFFF);

		uint8_t pixels[8*8*4]; // 8x8 RGBA
		for (int i = 0; i < 8*8*4; i++)
			pixels[i] = 255;

		m_DefaultTexture = std::make_shared<Texture>();
		m_DefaultTexture->InitFromPixels(8, 8, pixels);

		Core::Log::Info("Renderer initialized");
	}

	void Renderer::Shutdown()
	{
		m_MaterialCache.clear();
		m_GeometryCache.clear();

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
		if (!m_ActiveCamera)
			return;

		C3D_Mtx projection;
		Mtx_PerspTilt(&projection, C3D_AngleFromDegrees(m_ActiveCamera->fov), C3D_AspectRatioTop, 0.01f, 1000.0f, false);

		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, m_ULocProjection, &projection);

		for (const auto& renderable : m_Renderables)
		{
			Geometry& geometry = GetOrCreateGeometry(renderable.primitive);
			Material& material = GetOrCreateMaterial(renderable.primitive->GetMaterial().get());

			C3D_TexBind(0, material.GetBaseColorTexture()->GetTexture());
			//C3D_TexSetFilter(material.GetBaseColorTexture()->GetTexture(), GPU_LINEAR, GPU_NEAREST);

			C3D_Mtx modelView;

			glm::mat4 mv = m_ActiveCamera->GetViewMatrix() * renderable.transform;
			for (int row = 0; row < 4; row++)
			{
				modelView.r[row].c[0] = mv[3][row]; // w = col3
				modelView.r[row].c[1] = mv[2][row]; // z = col2
				modelView.r[row].c[2] = mv[1][row]; // y = col1
				modelView.r[row].c[3] = mv[0][row]; // x = col0
			}

			C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, m_ULocModelView, &modelView);

			C3D_BufInfo* bufInfo = C3D_GetBufInfo();
			BufInfo_Init(bufInfo);
			BufInfo_Add(bufInfo, geometry.GetVertexBuffer(), sizeof(PICA::Vertex), 2, 0x10);

			C3D_DrawElements(GPU_TRIANGLES, geometry.GetIndexCount(), C3D_UNSIGNED_SHORT, geometry.GetIndexBuffer());
		}
	}

	Geometry& Renderer::GetOrCreateGeometry(const Core::MeshPrimitive* primitive)
	{
		auto it = m_GeometryCache.find(primitive);
		if (it != m_GeometryCache.end())
			return it->second;

		// Create and add to cache if does not exist
		m_GeometryCache.emplace(primitive, Geometry(*primitive));
		return m_GeometryCache.at(primitive);
	}

	Material& Renderer::GetOrCreateMaterial(const Core::Material* material)
	{
		auto it = m_MaterialCache.find(material);
		if (it != m_MaterialCache.end())
			return it->second;

		std::shared_ptr<PICA::Texture> tex;
		if (material->baseColorTexture)
			tex = GetOrCreateTexture(material->baseColorTexture.get());
		else
			tex = m_DefaultTexture;

		// Create and add to cache if does not exist
		m_MaterialCache.emplace(material, Material(*material, tex));
		return m_MaterialCache.at(material);
	}

	std::shared_ptr<Texture> Renderer::GetOrCreateTexture(const Core::Texture* texture)
	{
		auto it = m_TextureCache.find(texture);
		if (it != m_TextureCache.end())
			return it->second;

		// Create and add to cache if does not exist
		auto tex = std::make_shared<PICA::Texture>(*texture);
		m_TextureCache.emplace(texture, tex);
		return m_TextureCache.at(texture);
	}
}
