#pragma once

namespace Voxen
{
	typedef enum class MouseCode : uint16_t
	{
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	} Mouse;

	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}

#define VOX_MOUSE_BUTTON_0      ::Voxen::Mouse::Button0
#define VOX_MOUSE_BUTTON_1      ::Voxen::Mouse::Button1
#define VOX_MOUSE_BUTTON_2      ::Voxen::Mouse::Button2
#define VOX_MOUSE_BUTTON_3      ::Voxen::Mouse::Button3
#define VOX_MOUSE_BUTTON_4      ::Voxen::Mouse::Button4
#define VOX_MOUSE_BUTTON_5      ::Voxen::Mouse::Button5
#define VOX_MOUSE_BUTTON_6      ::Voxen::Mouse::Button6
#define VOX_MOUSE_BUTTON_7      ::Voxen::Mouse::Button7
#define VOX_MOUSE_BUTTON_LAST   ::Voxen::Mouse::ButtonLast
#define VOX_MOUSE_BUTTON_LEFT   ::Voxen::Mouse::ButtonLeft
#define VOX_MOUSE_BUTTON_RIGHT  ::Voxen::Mouse::ButtonRight
#define VOX_MOUSE_BUTTON_MIDDLE ::Voxen::Mouse::ButtonMiddle