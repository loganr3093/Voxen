#pragma once

#include "Voxen/Core/Buffer.h"

namespace Voxen
{
	class FileSystem
	{
	public:
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
	};

}