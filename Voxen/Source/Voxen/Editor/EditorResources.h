#pragma once

#include "Voxen/Renderer/Texture.h"

namespace Voxen
{
	class EditorResources
	{
	public:
		// Shaders
		inline static std::filesystem::path FullScreenQuadShader = "Resources/Shaders/FullScreenQuad.glsl";
		inline static std::filesystem::path RaytraceShader = "Resources/Shaders/Raytrace.glsl";
		inline static std::filesystem::path SpriteRendererShader = "Resources/Shaders/SpriteRenderer.glsl";

		// Viewport
		inline static Ref<Texture2D> PlayIcon = nullptr;
		inline static Ref<Texture2D> StopIcon = nullptr;

		// Content Browser
		inline static Ref<Texture2D> FolderIcon = nullptr;
		inline static Ref<Texture2D> BlankFileIcon = nullptr;
		inline static Ref<Texture2D> TextFileIcon = nullptr;
		inline static Ref<Texture2D> CSFileIcon = nullptr;
		inline static Ref<Texture2D> VSCFileIcon = nullptr;
		inline static Ref<Texture2D> LuaFileIcon = nullptr;
		inline static Ref<Texture2D> BatFileIcon = nullptr;

		static void Init()
		{
			PlayIcon		= Texture2D::Create("Resources/Icons/PlayButton.png");
			StopIcon		= Texture2D::Create("Resources/Icons/StopButton.png");

			FolderIcon		= Texture2D::Create("Resources/Icons/ContentBrowser/FolderIcon.png");
			BlankFileIcon	= Texture2D::Create("Resources/Icons/ContentBrowser/BlankFileIcon.png");
			TextFileIcon	= Texture2D::Create("Resources/Icons/ContentBrowser/TextFileIcon.png");
			CSFileIcon		= Texture2D::Create("Resources/Icons/ContentBrowser/CSFileIcon.png");
			VSCFileIcon		= Texture2D::Create("Resources/Icons/ContentBrowser/VSCFileIcon.png");
			LuaFileIcon		= Texture2D::Create("Resources/Icons/ContentBrowser/LuaFileIcon.png");
			BatFileIcon		= Texture2D::Create("Resources/Icons/ContentBrowser/BatFileIcon.png");
		}

		static void Shutdown()
		{
			// Viewport
			PlayIcon.reset();
			StopIcon.reset();

			// Content Browser
			FolderIcon.reset();
			BlankFileIcon.reset();
			TextFileIcon.reset();
			CSFileIcon.reset();
			VSCFileIcon.reset();
			LuaFileIcon.reset();
			BatFileIcon.reset();
		}
	};
}