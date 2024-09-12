#pragma once

#include "Voxen/Project/Project.h"

namespace Voxen
{
	class ScriptBuilder
	{
	public:
		static void BuildCSProject(const std::filesystem::path& filepath);
		static void BuildScriptAssembly();
	};
}