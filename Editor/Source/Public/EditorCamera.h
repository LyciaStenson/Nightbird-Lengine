#pragma once

#include "Core/Camera.h"

namespace Nightbird
{
	class EditorCamera : public Camera
	{
	public:
		using Camera::Camera;
		~EditorCamera() override = default;

		void Tick(float delta) override;

		void SetLastMousePos(double x, double y);

	protected:
		float movementSpeed = 10.0f;
		float lookSensitivity = 0.005f;

		double lastX = 0.0, lastY = 0.0;
	};
}