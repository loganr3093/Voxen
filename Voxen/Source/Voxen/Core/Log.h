#pragma once

#include "Voxen/Core/Core.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Voxen
{
	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core log macros
#define VOX_CORE_TRACE(...)	::Voxen::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define VOX_CORE_INFO(...)	::Voxen::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VOX_CORE_WARN(...)	::Voxen::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VOX_CORE_ERROR(...)	::Voxen::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VOX_CORE_FATAL(...)	::Voxen::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define VOX_TRACE(...)		::Voxen::Log::GetClientLogger()->trace(__VA_ARGS__)
#define VOX_INFO(...)		::Voxen::Log::GetClientLogger()->info(__VA_ARGS__)
#define VOX_WARN(...)		::Voxen::Log::GetClientLogger()->warn(__VA_ARGS__)
#define VOX_ERROR(...)		::Voxen::Log::GetClientLogger()->error(__VA_ARGS__)
#define VOX_FATAL(...)		::Voxen::Log::GetClientLogger()->critical(__VA_ARGS__)