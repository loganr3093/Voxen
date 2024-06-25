#pragma once

#ifdef VOX_PLATFORM_WINDOWS
	#if VOX_DYNAMIC_LINK
		#ifdef VOX_BUILD_DLL
			#define VOXEN_API __declspec(dllexport)
		#else
			#define VOXEN_API __declspec(dllimport)
		#endif
	#else
		#define VOXEN_API	
	#endif
#else
	#error Voxen only supports Windows
#endif

#ifdef VOX_DEBUG
	#define VOX_ENABLE_ASSERTS
#endif

#ifdef HZ_ENABLE_ASSERTS
	#define VOX_ASSERT(x, ...) { if(!(x)) { HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define VOX_CORE_ASSERT(x, ...) { if(!(x)) { HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define VOX_ASSERT(x, ...)
	#define VOX_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)
#define VOX_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)