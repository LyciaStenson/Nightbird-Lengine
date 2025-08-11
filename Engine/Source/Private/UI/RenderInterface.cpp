#include <UI/RenderInterface.h>

namespace Nightbird
{
	Rml::CompiledGeometryHandle UIRenderInterface::CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices)
	{
		return 0;
	}

	void UIRenderInterface::RenderGeometry(Rml::CompiledGeometryHandle geometry, Rml::Vector2f translation, Rml::TextureHandle texture)
	{

	}

	void UIRenderInterface::ReleaseGeometry(Rml::CompiledGeometryHandle geometry)
	{

	}

	Rml::TextureHandle UIRenderInterface::LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source)
	{
		return 0;
	}

	Rml::TextureHandle UIRenderInterface::GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions)
	{
		return 0;
	}

	void UIRenderInterface::ReleaseTexture(Rml::TextureHandle texture)
	{

	}

	void UIRenderInterface::EnableScissorRegion(bool enable)
	{

	}

	void UIRenderInterface::SetScissorRegion(Rml::Rectanglei region)
	{

	}
}