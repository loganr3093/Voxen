#pragma once

// From glfw3.h

// Unknown
#define VOX_KEY_UNKNOWN			   -1  /* The unknown key*/

#define VOX_KEY_SPACE              32  /* Space key */
#define VOX_KEY_APOSTROPHE         39  /* Apostrophe (') key */
#define VOX_KEY_COMMA              44  /* Comma (,) key */
#define VOX_KEY_MINUS              45  /* Minus (-) key */
#define VOX_KEY_PERIOD             46  /* Period (.) key */
#define VOX_KEY_SLASH              47  /* Slash (/) key */
#define VOX_KEY_0                  48  /* Number 0 key */
#define VOX_KEY_1                  49  /* Number 1 key */
#define VOX_KEY_2                  50  /* Number 2 key */
#define VOX_KEY_3                  51  /* Number 3 key */
#define VOX_KEY_4                  52  /* Number 4 key */
#define VOX_KEY_5                  53  /* Number 5 key */
#define VOX_KEY_6                  54  /* Number 6 key */
#define VOX_KEY_7                  55  /* Number 7 key */
#define VOX_KEY_8                  56  /* Number 8 key */
#define VOX_KEY_9                  57  /* Number 9 key */
#define VOX_KEY_SEMICOLON          59  /* Semicolon (;) key */
#define VOX_KEY_EQUAL              61  /* Equal (=) key */
#define VOX_KEY_A                  65  /* Letter A key */
#define VOX_KEY_B                  66  /* Letter B key */
#define VOX_KEY_C                  67  /* Letter C key */
#define VOX_KEY_D                  68  /* Letter D key */
#define VOX_KEY_E                  69  /* Letter E key */
#define VOX_KEY_F                  70  /* Letter F key */
#define VOX_KEY_G                  71  /* Letter G key */
#define VOX_KEY_H                  72  /* Letter H key */
#define VOX_KEY_I                  73  /* Letter I key */
#define VOX_KEY_J                  74  /* Letter J key */
#define VOX_KEY_K                  75  /* Letter K key */
#define VOX_KEY_L                  76  /* Letter L key */
#define VOX_KEY_M                  77  /* Letter M key */
#define VOX_KEY_N                  78  /* Letter N key */
#define VOX_KEY_O                  79  /* Letter O key */
#define VOX_KEY_P                  80  /* Letter P key */
#define VOX_KEY_Q                  81  /* Letter Q key */
#define VOX_KEY_R                  82  /* Letter R key */
#define VOX_KEY_S                  83  /* Letter S key */
#define VOX_KEY_T                  84  /* Letter T key */
#define VOX_KEY_U                  85  /* Letter U key */
#define VOX_KEY_V                  86  /* Letter V key */
#define VOX_KEY_W                  87  /* Letter W key */
#define VOX_KEY_X                  88  /* Letter X key */
#define VOX_KEY_Y                  89  /* Letter Y key */
#define VOX_KEY_Z                  90  /* Letter Z key */
#define VOX_KEY_LEFT_BRACKET       91  /* Left bracket ([) key */
#define VOX_KEY_BACKSLASH          92  /* Backslash (\) key */
#define VOX_KEY_RIGHT_BRACKET      93  /* Right bracket (]) key */
#define VOX_KEY_GRAVE_ACCENT       96  /* Grave accent (`) key */
#define VOX_KEY_WORLD_1            161 /* Non-US #1 key */
#define VOX_KEY_WORLD_2            162 /* Non-US #2 key */

/* Function keys */
#define VOX_KEY_ESCAPE             256 /* Escape key */
#define VOX_KEY_ENTER              257 /* Enter key */
#define VOX_KEY_TAB                258 /* Tab key */
#define VOX_KEY_BACKSPACE          259 /* Backspace key */
#define VOX_KEY_INSERT             260 /* Insert key */
#define VOX_KEY_DELETE             261 /* Delete key */
#define VOX_KEY_RIGHT              262 /* Right arrow key */
#define VOX_KEY_LEFT               263 /* Left arrow key */
#define VOX_KEY_DOWN               264 /* Down arrow key */
#define VOX_KEY_UP                 265 /* Up arrow key */
#define VOX_KEY_PAGE_UP            266 /* Page Up key */
#define VOX_KEY_PAGE_DOWN          267 /* Page Down key */
#define VOX_KEY_HOME               268 /* Home key */
#define VOX_KEY_END                269 /* End key */
#define VOX_KEY_CAPS_LOCK          280 /* Caps Lock key */
#define VOX_KEY_SCROLL_LOCK        281 /* Scroll Lock key */
#define VOX_KEY_NUM_LOCK           282 /* Num Lock key */
#define VOX_KEY_PRINT_SCREEN       283 /* Print Screen key */
#define VOX_KEY_PAUSE              284 /* Pause key */
#define VOX_KEY_F1                 290 /* F1 key */
#define VOX_KEY_F2                 291 /* F2 key */
#define VOX_KEY_F3                 292 /* F3 key */
#define VOX_KEY_F4                 293 /* F4 key */
#define VOX_KEY_F5                 294 /* F5 key */
#define VOX_KEY_F6                 295 /* F6 key */
#define VOX_KEY_F7                 296 /* F7 key */
#define VOX_KEY_F8                 297 /* F8 key */
#define VOX_KEY_F9                 298 /* F9 key */
#define VOX_KEY_F10                299 /* F10 key */
#define VOX_KEY_F11                300 /* F11 key */
#define VOX_KEY_F12                301 /* F12 key */
#define VOX_KEY_F13                302 /* F13 key */
#define VOX_KEY_F14                303 /* F14 key */
#define VOX_KEY_F15                304 /* F15 key */
#define VOX_KEY_F16                305 /* F16 key */
#define VOX_KEY_F17                306 /* F17 key */
#define VOX_KEY_F18                307 /* F18 key */
#define VOX_KEY_F19                308 /* F19 key */
#define VOX_KEY_F20                309 /* F20 key */
#define VOX_KEY_F21                310 /* F21 key */
#define VOX_KEY_F22                311 /* F22 key */
#define VOX_KEY_F23                312 /* F23 key */
#define VOX_KEY_F24                313 /* F24 key */
#define VOX_KEY_F25                314 /* F25 key */
#define VOX_KEY_KP_0               320 /* Keypad 0 key */
#define VOX_KEY_KP_1               321 /* Keypad 1 key */
#define VOX_KEY_KP_2               322 /* Keypad 2 key */
#define VOX_KEY_KP_3               323 /* Keypad 3 key */
#define VOX_KEY_KP_4               324 /* Keypad 4 key */
#define VOX_KEY_KP_5               325 /* Keypad 5 key */
#define VOX_KEY_KP_6               326 /* Keypad 6 key */
#define VOX_KEY_KP_7               327 /* Keypad 7 key */
#define VOX_KEY_KP_8               328 /* Keypad 8 key */
#define VOX_KEY_KP_9               329 /* Keypad 9 key */
#define VOX_KEY_KP_DECIMAL         330 /* Keypad decimal key */
#define VOX_KEY_KP_DIVIDE          331 /* Keypad divide key */
#define VOX_KEY_KP_MULTIPLY        332 /* Keypad multiply key */
#define VOX_KEY_KP_SUBTRACT        333 /* Keypad subtract key */
#define VOX_KEY_KP_ADD             334 /* Keypad add key */
#define VOX_KEY_KP_ENTER           335 /* Keypad enter key */
#define VOX_KEY_KP_EQUAL           336 /* Keypad equal key */
#define VOX_KEY_LEFT_SHIFT         340 /* Left Shift key */
#define VOX_KEY_LEFT_CONTROL       341 /* Left Control key */
#define VOX_KEY_LEFT_ALT           342 /* Left Alt key */
#define VOX_KEY_LEFT_SUPER         343 /* Left Super key */
#define VOX_KEY_RIGHT_SHIFT        344 /* Right Shift key */
#define VOX_KEY_RIGHT_CONTROL      345 /* Right Control key */
#define VOX_KEY_RIGHT_ALT          346 /* Right Alt key */
#define VOX_KEY_RIGHT_SUPER        347 /* Right Super key */
#define VOX_KEY_MENU               348 /* Menu key */