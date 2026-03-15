#pragma once

#include <gx2r/buffer.h>

#include <cstdint>

namespace Nightbird::Core
{
	class MeshPrimitive;
}

namespace Nightbird::WiiU
{
	class Geometry
	{
	public:
		Geometry() = default;
		Geometry(const Core::MeshPrimitive& primitive);
		~Geometry();

		Geometry(const Geometry&) = delete;
		Geometry& operator=(const Geometry&) = delete;
		Geometry(Geometry&& other) noexcept;
		Geometry& operator=(Geometry&& other) noexcept;

		GX2RBuffer& GetPositionBuffer();
		GX2RBuffer& GetIndexBuffer();
		uint32_t GetIndexCount() const;

	private:
		void CreatePositionBuffer(const Core::MeshPrimitive& primitive);
		void CreateIndexBuffer(const Core::MeshPrimitive& primitive);

		GX2RBuffer m_PositionBuffer = {};
		GX2RBuffer m_IndexBuffer = {};
		uint32_t m_IndexCount = 0;
	};
}
