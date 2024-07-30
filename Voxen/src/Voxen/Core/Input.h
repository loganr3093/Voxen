#pragma once

#include "Voxen/Core/Core.h"
#include "Voxen/Core/KeyCodes.h"
#include "Voxen/Core/MouseCodes.h"

namespace Voxen
{

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};

}