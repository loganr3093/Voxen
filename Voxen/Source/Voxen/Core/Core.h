#pragma once

#include <memory>

#include "Voxen/Core/PlatformDetection.h"

#ifdef VOX_DEBUG
	#if defined(VOX_PLATFORM_WINDOWS)
		#define VOX_DEBUGBREAK() __debugbreak()
	#elif defined(VOX_PLATFORM_LINUX)
		#include <signal.h>
		#define VOX_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
		#define VOX_ENABLE_ASSERTS
#else
	#define VOX_DEBUGBREAK()
#endif

#define VOX_EXPAND_MACRO(x) x
#define VOX_STRINGIFY_MACRO(x) #x
#define BIT(x) (1 << x)
#define VOX_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Voxen
{

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}

#include "Voxen/Core/Log.h"
#include "Voxen/Core/Assert.h"