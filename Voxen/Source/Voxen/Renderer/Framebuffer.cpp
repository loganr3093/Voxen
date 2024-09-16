#include "voxpch.h"
#include "Voxen/Renderer/Framebuffer.h"

#include "Voxen/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Voxen
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: VOX_CORE_ASSERT("RendererAPI::None is currently not supported"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(spec);
		}

		VOX_CORE_ASSERT("Unknown RenderAPI");
		return nullptr;
	}
}