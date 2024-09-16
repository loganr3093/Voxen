#pragma once

#include "Voxen/Renderer/RenderCommand.h"
#include "Voxen/Renderer/Shader.h"
#include "Voxen/Renderer/EditorCamera.h"

#include "Voxen/Scene/Scene.h"
#include "Voxen/Scene/Components.h"

namespace Voxen
{
	enum class RendererType
	{
		None = 0,
		Renderer2D, VoxRenderer
	};

	class Renderer
	{
	public:
		static void Init(RendererType type);
		static void Shutdown();

		static void OnWindowResize(uint32 width, uint32 height);

		static void BeginScene(const Camera& camera, const Matrix4& cameraTransform);
		static void BeginScene(const EditorCamera& camera);
		static void EndScene();

		static void RenderScene(Ref<Scene> scene);

		static RendererType GetType();
	};
}

