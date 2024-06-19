#pragma once

#ifdef VOX_PLATFORM_WINDOWS
	#ifdef VOX_BUILD_DLL
		#define VOXEN_API __declspec(dllexport)
	#else
		#define VOXEN_API __declspec(dllimport)
	#endif // !VOX_BUILD_DLL
#else
	#error Voxen only supports Windows
#endif // !VOX_PLATFORM_WINDOWS

