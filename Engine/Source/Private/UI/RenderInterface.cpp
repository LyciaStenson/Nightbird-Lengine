#include <UI/RenderInterface.h>

#include <Core/Renderer.h>

namespace Nightbird
{
	UIRenderInterface::UIRenderInterface(Renderer* renderer)
	{

	}

	UIRenderInterface::~UIRenderInterface()
	{

	}
	
	/// Called by RmlUi when it wants to compile geometry it believes will be static for the forseeable future.
	Rml::CompiledGeometryHandle UIRenderInterface::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices)
	{
		return Rml::CompiledGeometryHandle();
	}

	/// Called by RmlUi when it wants to render application-compiled geometry.
	void UIRenderInterface::RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture)
	{

	}

	/// Called by RmlUi when it wants to release application-compiled geometry.
	void UIRenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry)
	{

	}

	/// Called by RmlUi when a texture is required by the library.
	Rml::TextureHandle UIRenderInterface::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source)
	{
		return Rml::TextureHandle();
	}

	/// Called by RmlUi when a texture is required to be built from an internally-generated sequence of pixels.
	Rml::TextureHandle UIRenderInterface::GenerateTexture(Rml::Span<const Rml::byte> source_data, Rml::Vector2i source_dimensions)
	{
		return Rml::TextureHandle();
	}
	
	/// Called by RmlUi when a loaded texture is no longer required.
	void UIRenderInterface::ReleaseTexture(Rml::TextureHandle texture_handle)
	{

	}

	/// Called by RmlUi when it wants to enable or disable scissoring to clip content.
	void UIRenderInterface::EnableScissorRegion(bool enable)
	{

	}

	/// Called by RmlUi when it wants to change the scissor region.
	void UIRenderInterface::SetScissorRegion(Rml::Rectanglei region)
	{

	}

	/// Called by RmlUi when it wants to set the current transform matrix to a new matrix.
	void UIRenderInterface::SetTransform(const Rml::Matrix4f* transform)
	{

	}
}