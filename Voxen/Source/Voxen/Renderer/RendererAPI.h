#pragma once

#include "Voxen/Renderer/VertexArray.h"

#include <glm/glm.hpp>

namespace Voxen
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) = 0;
		virtual void SetClearColor(const Vector4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32 indexCount = 0) = 0;

		inline static API GetAPI() { return s_API; }
		static Scope<RendererAPI> Create();
	private:
		static API s_API;
	};
}

