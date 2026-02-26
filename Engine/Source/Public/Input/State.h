#pragma once

#include <glm/glm.hpp>

#include <array>

namespace Nightbird::Input
{
	enum class Key : uint16_t
	{
		Space, Enter, Escape,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		UNKNOWN, SIZE
	};

	enum class Button : uint16_t
	{
		A, B, X, Y, L, R, ZL, ZR,
		Plus,
		Start = Plus,
		Minus,
		Back = Minus,
		Home,
		Up, Down, Left, Right,
		UNKNOWN, SIZE
	};

	enum class Axis : uint16_t
	{
		Left, Right,
		UNKNOWN, SIZE
	};

	struct State
	{
		std::array<bool, static_cast<size_t>(Key::SIZE)> keysDown;
		std::array<bool, static_cast<size_t>(Key::SIZE)> keysPressed;
		std::array<bool, static_cast<size_t>(Key::SIZE)> keysReleased;

		std::array<bool, static_cast<size_t>(Button::SIZE)> buttonsDown;
		std::array<bool, static_cast<size_t>(Button::SIZE)> buttonsPressed;
		std::array<bool, static_cast<size_t>(Button::SIZE)> buttonsReleased;

		std::array<glm::vec2, static_cast<size_t>(Axis::SIZE)> axes;
	};
}