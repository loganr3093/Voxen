#pragma once

#include "Voxen/Core/Core.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

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

		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
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