#pragma once

#include <RmlUi/Core/RenderInterface.h>

#include <volk.h>
#include <vk_mem_alloc.h>

namespace Nightbird
{
	class Renderer;

	class UIRenderInterface : public Rml::RenderInterface
	{
	public:
		UIRenderInterface(Renderer* renderer);
		~UIRenderInterface();

		// -- Inherited from Rml::RenderInterface --

		/// Called by RmlUi when it wants to compile geometry it believes will be static for the forseeable future.
		Rml::CompiledGeometryHandle CompileGeometry(Rml::Span<const Rml::Vertex> vertices, Rml::Span<const int> indices) override;
		/// Called by RmlUi when it wants to render application-compiled geometry.
		void RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture) override;
		/// Called by RmlUi when it wants to release application-compiled geometry.
		void ReleaseGeometry(Rml::CompiledGeometryHandle geometry) override;

		/// Called by RmlUi when a texture is required by the library.
		Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
		/// Called by RmlUi when a texture is required to be built from an internally-generated sequence of pixels.
		Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source_data, Rml::Vector2i source_dimensions) override;
		/// Called by RmlUi when a loaded texture is no longer required.
		void ReleaseTexture(Rml::TextureHandle texture_handle) override;

		/// Called by RmlUi when it wants to enable or disable scissoring to clip content.
		void EnableScissorRegion(bool enable) override;
		/// Called by RmlUi when it wants to change the scissor region.
		void SetScissorRegion(Rml::Rectanglei region) override;

		/// Called by RmlUi when it wants to set the current transform matrix to a new matrix.
		void SetTransform(const Rml::Matrix4f* transform) override;
	};
}