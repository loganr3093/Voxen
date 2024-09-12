#pragma once

#include <string>
#include <filesystem>

namespace Voxen
{
	class FileDialogs
	{
	public:
		// These return empty strings if cancelled
		static std::filesystem::path OpenFile(const char* filter);
		static std::filesystem::path SaveFile(const char* filter);
		static std::filesystem::path OpenFolder();
	};

	class Time
	{
	public:
		static float GetTime();
	};
}