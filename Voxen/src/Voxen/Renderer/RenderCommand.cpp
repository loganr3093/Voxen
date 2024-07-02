#include "voxpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Voxen
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}