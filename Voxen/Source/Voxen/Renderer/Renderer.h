#pragma once

#include "Voxen/Renderer/RenderCommand.h"
#include "Voxen/Renderer/OrthographicCamera.h"
#include "Voxen/Renderer/Shader.h"

namespace Voxen
{
	class Renderer
	{
	public:
		static void Init();
		static void OnWindowResize(uint32 width, uint32 height);

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const Matrix4& transform = Matrix4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			Matrix4 ViewProjectionMatrix;
		};

		static Scope<SceneData> s_SceneData;
	};
}

