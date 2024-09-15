#pragma once

#include "Voxen/Renderer/OrthographicCamera.h"
#include "Voxen/Renderer/Texture.h"
#include "Voxen/Renderer/Camera.h"
#include "Voxen/Renderer/EditorCamera.h"

#include "Voxen/Scene/Components.h"

namespace Voxen
{
	class VoxRenderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginScene(const EditorCamera& camera);
		static void EndScene();
	};
}