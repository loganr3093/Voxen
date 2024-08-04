#include "voxpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Voxen
{
	Ref<VertexBuffer> VertexBuffer::Create(uint32 size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			VOX_CORE_ASSERT(false, "RendererAPI::None is currently not suported");
			return nullptr;
			break;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexBuffer>(size);
			break;
		}

		VOX_CORE_ASSERT(false, "Unknown renderAPI");
		return nullptr;
	}
	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32 size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			VOX_CORE_ASSERT(false, "RendererAPI::None is currently not suported");
			return nullptr;
			break;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexBuffer>(vertices, size);
			break;
		}

		VOX_CORE_ASSERT(false, "Unknown renderAPI");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32* indices, uint32 size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			VOX_CORE_ASSERT(false, "RendererAPI::None is currently not suported");
			return nullptr;
			break;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLIndexBuffer>(indices, size);
			break;
		}

		VOX_CORE_ASSERT(false, "Unknown renderAPI");
		return nullptr;
	}
}