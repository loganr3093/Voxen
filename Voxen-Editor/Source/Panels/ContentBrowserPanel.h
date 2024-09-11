#pragma once

#include "Voxen/Renderer/Texture.h"

#include <filesystem>

namespace Voxen
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;
	};
}

