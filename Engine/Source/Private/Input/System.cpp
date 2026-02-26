#include "Input/System.h"

#include <glm/glm.hpp>

namespace Nightbird::Input
{
	void System::Update(Provider& provider)
	{
		provider.Poll(m_State);
	}

	bool System::IsKeyDown(Key key) const
	{
		return m_State.keysDown[static_cast<size_t>(key)];
	}

	bool System::WasKeyPressed(Key key) const
	{
		return m_State.keysPressed[static_cast<size_t>(key)];
	}
	
	bool System::WasKeyReleased(Key key) const
	{
		return m_State.keysReleased[static_cast<size_t>(key)];
	}
	
	bool System::IsButtonDown(Button button) const
	{
		return m_State.buttonsDown[static_cast<size_t>(button)];
	}
	
	bool System::WasButtonPressed(Button button) const
	{
		return m_State.buttonsPressed[static_cast<size_t>(button)];
	}
	
	bool System::WasButtonReleased(Button button) const
	{
		return m_State.buttonsReleased[static_cast<size_t>(button)];
	}

	glm::vec2 System::GetAxis(Axis axis) const
	{
		return m_State.axes[static_cast<size_t>(axis)];
	}
}