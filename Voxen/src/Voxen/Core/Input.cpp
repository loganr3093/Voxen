#include "voxpch.h"
#include "Voxen/Core/Input.h"

#ifdef VOX_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsInput.h"
#endif

namespace Voxen {

	Scope<Input> Input::s_Instance = Input::Create();

	Scope<Input> Input::Create()
	{
#ifdef VOX_PLATFORM_WINDOWS
		return CreateScope<WindowsInput>();
#else
		VOX_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}