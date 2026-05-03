#pragma once

#include "Input/InputState.h"

namespace Nightbird::Input
{
	class Provider;

	class System
	{
	public:
		System() = default;
		~System() = default;

		void Update(Provider& provider);

		bool IsKeyDown(Key key) const;
		bool WasKeyPressed(Key key) const;
		bool WasKeyReleased(Key key) const;

		bool IsButtonDown(Button button) const;
		bool WasButtonPressed(Button button) const;
		bool WasButtonReleased(Button button) const;

		glm::vec2 GetAxis(Axis axis) const;

	private:
		State m_State;
	};
}
