#include "voxpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Voxen
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32 size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			VOX_CORE_ASSERT(false, "RendererAPI::None is currently not suported");
			return nullptr;
			break;
		case RendererAPI::API::OpenGL:
			return new OpenGLVertexBuffer(vertices, size);
			break;
		}

		VOX_CORE_ASSERT(false, "Unknown renderAPI");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32* indices, uint32 size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			VOX_CORE_ASSERT(false, "RendererAPI::None is currently not suported");
			return nullptr;
			break;
		case RendererAPI::API::OpenGL:
			return new OpenGLIndexBuffer(indices, size);
			break;
		}

		VOX_CORE_ASSERT(false, "Unknown renderAPI");
		return nullptr;
	}
}