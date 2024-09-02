#pragma once

#include "Voxen/Core/PlatformDetection.h"

#ifdef VOX_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
#endif

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Voxen/Core/Core.h"
#include "Voxen/Core/Log.h"
#include "Voxen/Debug/Instrumentor.h"
#include "Voxen/Types/Types.h"

#ifdef VOX_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
