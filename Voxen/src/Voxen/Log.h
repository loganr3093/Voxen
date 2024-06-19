#pragma once

#include <memory>

#include "Core.h"
#include "spdlog/spdlog.h"

namespace Voxen
{
	class VOXEN_API Log
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
#define VOX_CORE_FATAL(...)	::Voxen::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define VOX_TRACE(...)		::Voxen::Log::GetClientLogger()->trace(__VA_ARGS__)
#define VOX_INFO(...)		::Voxen::Log::GetClientLogger()->info(__VA_ARGS__)
#define VOX_WARN(...)		::Voxen::Log::GetClientLogger()->warn(__VA_ARGS__)
#define VOX_ERROR(...)		::Voxen::Log::GetClientLogger()->error(__VA_ARGS__)
#define VOX_FATAL(...)		::Voxen::Log::GetClientLogger()->fatal(__VA_ARGS__)