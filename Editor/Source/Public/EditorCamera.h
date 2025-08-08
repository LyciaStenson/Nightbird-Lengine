#pragma once

#include <Core/Camera.h>

namespace Nightbird
{
	class EditorCamera : public Camera
	{
	public:
		using Camera::Camera;
		~EditorCamera() override = default;

		void Tick(float delta) override;

	protected:
		float movementSpeed = 10.0f;
		float lookSensitivity = 0.01f;
	};
}