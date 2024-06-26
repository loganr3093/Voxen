#pragma once

#include "voxpch.h"

#include "Voxen/Core.h"
#include "Voxen/Events/Event.h"

namespace Voxen
{
	struct WindowProps
	{
		std::string Title;
		uint32 Width;
		uint32 Height;

		WindowProps(
			const std::string& title = "Voxen Engine",
			uint32 width = 1280,
			uint32 height = 720)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// Interface representing a desktop system based Window
	class VOXEN_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};

}