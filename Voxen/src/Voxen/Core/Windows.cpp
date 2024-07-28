#include "voxpch.h"
#include "Voxen/Core/Window.h"

#ifdef VOX_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Voxen
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
#ifdef VOX_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
#else
		VOX_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

}