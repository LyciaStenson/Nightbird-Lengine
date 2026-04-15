#pragma once

#include "ImGuiWindow.h"

//#include <rttr/type.h>

namespace Nightbird::Editor
{
	class EditorContext;

	class Inspector : public ImGuiWindow
	{
	public:
		//NB_OBJECT()

		Inspector(EditorContext& context, bool open = true);

	protected:
		void OnRender() override;

	private:
		//void DrawProperties(rttr::instance instance);

		EditorContext& m_Context;
	};
}
