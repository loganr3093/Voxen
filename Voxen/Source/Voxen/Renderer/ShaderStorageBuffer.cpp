#include "voxpch.h"
#include "Voxen/Renderer/ShaderStorageBuffer.h"

#include "Voxen/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLShaderStorageBuffer.h"

namespace Voxen
{
	Ref<ShaderStorageBuffer> ShaderStorageBuffer::Create(const void* data, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: VOX_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLShaderStorageBuffer>(data, size);
		}

		VOX_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}