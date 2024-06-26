#pragma once

#include "Voxen/Input.h"

namespace Voxen
{
	class WindowsInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl(int32 keycode) override;

		virtual bool IsMouseButtonPressedImpl(int32 button) override;
		virtual std::pair<float, float> GetMousePositionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
	};
}