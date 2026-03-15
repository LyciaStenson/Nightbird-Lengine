#pragma once

#include "Core/Platform.h"

#include "InputProvider.h"

namespace Nightbird::WiiU
{
	class Platform : public Core::Platform
	{
	public:
		Platform();
		virtual void Initialize() override;
		virtual void Shutdown() override;
		virtual void PollEvents() override;
		virtual void WaitEvents() override;
		virtual bool ShouldClose() const override;
		virtual void GetFramebufferSize(int* width, int* height) const override;

		virtual std::filesystem::path GetCookedAssetsPath() const override;

		Input::Provider& GetInputProvider() override;

	private:
		InputProvider m_InputProvider;
	};
}
