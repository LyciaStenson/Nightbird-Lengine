#pragma once

namespace Nightbird
{
	class MeshInstance;
	class MeshPrimitive;

	struct Renderable
	{
		MeshInstance* instance;
		MeshPrimitive* primitive;
	};
}