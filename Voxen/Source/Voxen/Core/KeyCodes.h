#pragma once

namespace Voxen
{
	typedef enum class KeyCode : uint16_t
	{
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

// From glfw3.h
#define VOX_KEY_SPACE           ::Voxen::Key::Space
#define VOX_KEY_APOSTROPHE      ::Voxen::Key::Apostrophe    /* ' */
#define VOX_KEY_COMMA           ::Voxen::Key::Comma         /* , */
#define VOX_KEY_MINUS           ::Voxen::Key::Minus         /* - */
#define VOX_KEY_PERIOD          ::Voxen::Key::Period        /* . */
#define VOX_KEY_SLASH           ::Voxen::Key::Slash         /* / */
#define VOX_KEY_0               ::Voxen::Key::D0
#define VOX_KEY_1               ::Voxen::Key::D1
#define VOX_KEY_2               ::Voxen::Key::D2
#define VOX_KEY_3               ::Voxen::Key::D3
#define VOX_KEY_4               ::Voxen::Key::D4
#define VOX_KEY_5               ::Voxen::Key::D5
#define VOX_KEY_6               ::Voxen::Key::D6
#define VOX_KEY_7               ::Voxen::Key::D7
#define VOX_KEY_8               ::Voxen::Key::D8
#define VOX_KEY_9               ::Voxen::Key::D9
#define VOX_KEY_SEMICOLON       ::Voxen::Key::Semicolon     /* ; */
#define VOX_KEY_EQUAL           ::Voxen::Key::Equal         /* = */
#define VOX_KEY_A               ::Voxen::Key::A
#define VOX_KEY_B               ::Voxen::Key::B
#define VOX_KEY_C               ::Voxen::Key::C
#define VOX_KEY_D               ::Voxen::Key::D
#define VOX_KEY_E               ::Voxen::Key::E
#define VOX_KEY_F               ::Voxen::Key::F
#define VOX_KEY_G               ::Voxen::Key::G
#define VOX_KEY_H               ::Voxen::Key::H
#define VOX_KEY_I               ::Voxen::Key::I
#define VOX_KEY_J               ::Voxen::Key::J
#define VOX_KEY_K               ::Voxen::Key::K
#define VOX_KEY_L               ::Voxen::Key::L
#define VOX_KEY_M               ::Voxen::Key::M
#define VOX_KEY_N               ::Voxen::Key::N
#define VOX_KEY_O               ::Voxen::Key::O
#define VOX_KEY_P               ::Voxen::Key::P
#define VOX_KEY_Q               ::Voxen::Key::Q
#define VOX_KEY_R               ::Voxen::Key::R
#define VOX_KEY_S               ::Voxen::Key::S
#define VOX_KEY_T               ::Voxen::Key::T
#define VOX_KEY_U               ::Voxen::Key::U
#define VOX_KEY_V               ::Voxen::Key::V
#define VOX_KEY_W               ::Voxen::Key::W
#define VOX_KEY_X               ::Voxen::Key::X
#define VOX_KEY_Y               ::Voxen::Key::Y
#define VOX_KEY_Z               ::Voxen::Key::Z
#define VOX_KEY_LEFT_BRACKET    ::Voxen::Key::LeftBracket   /* [ */
#define VOX_KEY_BACKSLASH       ::Voxen::Key::Backslash     /* \ */
#define VOX_KEY_RIGHT_BRACKET   ::Voxen::Key::RightBracket  /* ] */
#define VOX_KEY_GRAVE_ACCENT    ::Voxen::Key::GraveAccent   /* ` */
#define VOX_KEY_WORLD_1         ::Voxen::Key::World1        /* non-US #1 */
#define VOX_KEY_WORLD_2         ::Voxen::Key::World2        /* non-US #2 */

/* Function keys */
#define VOX_KEY_ESCAPE          ::Voxen::Key::Escape
#define VOX_KEY_ENTER           ::Voxen::Key::Enter
#define VOX_KEY_TAB             ::Voxen::Key::Tab
#define VOX_KEY_BACKSPACE       ::Voxen::Key::Backspace
#define VOX_KEY_INSERT          ::Voxen::Key::Insert
#define VOX_KEY_DELETE          ::Voxen::Key::Delete
#define VOX_KEY_RIGHT           ::Voxen::Key::Right
#define VOX_KEY_LEFT            ::Voxen::Key::Left
#define VOX_KEY_DOWN            ::Voxen::Key::Down
#define VOX_KEY_UP              ::Voxen::Key::Up
#define VOX_KEY_PAGE_UP         ::Voxen::Key::PageUp
#define VOX_KEY_PAGE_DOWN       ::Voxen::Key::PageDown
#define VOX_KEY_HOME            ::Voxen::Key::Home
#define VOX_KEY_END             ::Voxen::Key::End
#define VOX_KEY_CAPS_LOCK       ::Voxen::Key::CapsLock
#define VOX_KEY_SCROLL_LOCK     ::Voxen::Key::ScrollLock
#define VOX_KEY_NUM_LOCK        ::Voxen::Key::NumLock
#define VOX_KEY_PRINT_SCREEN    ::Voxen::Key::PrintScreen
#define VOX_KEY_PAUSE           ::Voxen::Key::Pause
#define VOX_KEY_F1              ::Voxen::Key::F1
#define VOX_KEY_F2              ::Voxen::Key::F2
#define VOX_KEY_F3              ::Voxen::Key::F3
#define VOX_KEY_F4              ::Voxen::Key::F4
#define VOX_KEY_F5              ::Voxen::Key::F5
#define VOX_KEY_F6              ::Voxen::Key::F6
#define VOX_KEY_F7              ::Voxen::Key::F7
#define VOX_KEY_F8              ::Voxen::Key::F8
#define VOX_KEY_F9              ::Voxen::Key::F9
#define VOX_KEY_F10             ::Voxen::Key::F10
#define VOX_KEY_F11             ::Voxen::Key::F11
#define VOX_KEY_F12             ::Voxen::Key::F12
#define VOX_KEY_F13             ::Voxen::Key::F13
#define VOX_KEY_F14             ::Voxen::Key::F14
#define VOX_KEY_F15             ::Voxen::Key::F15
#define VOX_KEY_F16             ::Voxen::Key::F16
#define VOX_KEY_F17             ::Voxen::Key::F17
#define VOX_KEY_F18             ::Voxen::Key::F18
#define VOX_KEY_F19             ::Voxen::Key::F19
#define VOX_KEY_F20             ::Voxen::Key::F20
#define VOX_KEY_F21             ::Voxen::Key::F21
#define VOX_KEY_F22             ::Voxen::Key::F22
#define VOX_KEY_F23             ::Voxen::Key::F23
#define VOX_KEY_F24             ::Voxen::Key::F24
#define VOX_KEY_F25             ::Voxen::Key::F25

/* Keypad */
#define VOX_KEY_KP_0            ::Voxen::Key::KP0
#define VOX_KEY_KP_1            ::Voxen::Key::KP1
#define VOX_KEY_KP_2            ::Voxen::Key::KP2
#define VOX_KEY_KP_3            ::Voxen::Key::KP3
#define VOX_KEY_KP_4            ::Voxen::Key::KP4
#define VOX_KEY_KP_5            ::Voxen::Key::KP5
#define VOX_KEY_KP_6            ::Voxen::Key::KP6
#define VOX_KEY_KP_7            ::Voxen::Key::KP7
#define VOX_KEY_KP_8            ::Voxen::Key::KP8
#define VOX_KEY_KP_9            ::Voxen::Key::KP9
#define VOX_KEY_KP_DECIMAL      ::Voxen::Key::KPDecimal
#define VOX_KEY_KP_DIVIDE       ::Voxen::Key::KPDivide
#define VOX_KEY_KP_MULTIPLY     ::Voxen::Key::KPMultiply
#define VOX_KEY_KP_SUBTRACT     ::Voxen::Key::KPSubtract
#define VOX_KEY_KP_ADD          ::Voxen::Key::KPAdd
#define VOX_KEY_KP_ENTER        ::Voxen::Key::KPEnter
#define VOX_KEY_KP_EQUAL        ::Voxen::Key::KPEqual

#define VOX_KEY_LEFT_SHIFT      ::Voxen::Key::LeftShift
#define VOX_KEY_LEFT_CONTROL    ::Voxen::Key::LeftControl
#define VOX_KEY_LEFT_ALT        ::Voxen::Key::LeftAlt
#define VOX_KEY_LEFT_SUPER      ::Voxen::Key::LeftSuper
#define VOX_KEY_RIGHT_SHIFT     ::Voxen::Key::RightShift
#define VOX_KEY_RIGHT_CONTROL   ::Voxen::Key::RightControl
#define VOX_KEY_RIGHT_ALT       ::Voxen::Key::RightAlt
#define VOX_KEY_RIGHT_SUPER     ::Voxen::Key::RightSuper
#define VOX_KEY_MENU            ::Voxen::Key::Menu