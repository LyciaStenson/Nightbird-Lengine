#pragma once

#include "ImGuiWindow.h"

namespace Nightbird::Editor
{
	class EditorContext;

	class Inspector : public ImGuiWindow
	{
	public:
		NB_TYPE()

		Inspector(EditorContext& context, bool open = true);

	protected:
		void OnRender() override;

	private:
		void DrawFields(TypeInfo* type);

		EditorContext& m_Context;
	};
}
