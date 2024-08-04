#pragma once

#include "Voxen/Renderer/RendererAPI.h"

namespace Voxen
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32 indexCount = 0) override;
	};
}