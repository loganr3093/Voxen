#include "voxpch.h"
#include "Voxen/Renderer/VertexArray.h"

#include "Voxen/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Voxen
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None: VOX_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexArray>();
		}

		VOX_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}