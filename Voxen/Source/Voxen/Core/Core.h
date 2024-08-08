#pragma once
#include <memory>

// Platform detection
#ifdef _WIN32
	#ifdef _WIN64
		#define VOX_PLATFORM_WINDOWS
	#else
		#error "x86 builds are not supported"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	/* TARGET_OS_MAC exists on all the platforms
	 * so we must check all of them (in this order)
	 * to ensure that we're running on MAC
	 * and not some other Apple platform */
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		#define HZ_PLATFORM_IOS
		#error "IOS is not supported!"
	#elif TARGET_OS_MAC == 1
		#define VOX_PLATFORM_MACOS
		#error "MacOS is not supported!"
	#else
		#error "Unknown Apple platform!"
	#endif
		/* We also have to check __ANDROID__ before __linux__
		 * since android is based on the linux kernel
		 * it has __linux__ defined */
	#elif defined(__ANDROID__)
		#define HZ_PLATFORM_ANDROID
		#error "Android is not supported!"
	#elif defined(__linux__)
		#define HZ_PLATFORM_LINUX
		#error "Linux is not supported!"
	#else
		/* Unknown compiler/platform */
		#error "Unknown platform!"
#endif // End of platform detection

#ifdef VOX_PLATFORM_WINDOWS
	// Nothing
#else
	#error Voxen only supports Windows
#endif

#ifdef VOX_DEBUG
	#define VOX_ENABLE_ASSERTS
#endif

#ifdef VOX_ENABLE_ASSERTS
	#define VOX_ASSERT(x, ...) { if(!(x)) { VOX_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define VOX_CORE_ASSERT(x, ...) { if(!(x)) { VOX_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define VOX_ASSERT(x, ...)
	#define VOX_CORE_ASSERT(x, ...)
#endif

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