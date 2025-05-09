#include "voxpch.h"
#include "Voxen/Renderer/GraphicsContext.h"

#include "Voxen/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace Voxen
{

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    VOX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		VOX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}