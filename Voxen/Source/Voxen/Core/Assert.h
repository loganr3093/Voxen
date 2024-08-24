#pragma once

#include "Voxen/Core/Core.h"
#include "Voxen/Core/Log.h"

#include <filesystem>

#ifdef VOX_ENABLE_ASSERTS
	// Alternatively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define VOX_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { VOX##type##ERROR(msg, __VA_ARGS__); VOX_DEBUGBREAK(); } }
	#define VOX_INTERNAL_ASSERT_WITH_MSG(type, check, ...) VOX_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define VOX_INTERNAL_ASSERT_NO_MSG(type, check) VOX_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", VOX_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define VOX_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define VOX_INTERNAL_ASSERT_GET_MACRO(...) VOX_EXPAND_MACRO( VOX_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, VOX_INTERNAL_ASSERT_WITH_MSG, VOX_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define VOX_ASSERT(...) VOX_EXPAND_MACRO( VOX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define VOX_CORE_ASSERT(...) VOX_EXPAND_MACRO( VOX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define VOX_ASSERT(...)
	#define VOX_CORE_ASSERT(...)
#endif