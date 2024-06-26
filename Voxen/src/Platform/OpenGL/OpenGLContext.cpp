#include "voxpch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Voxen
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle)
	{
		VOX_CORE_ASSERT(windowHandle, "Window handle is null");
	}
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int32 status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		VOX_CORE_ASSERT(status, "Failed to initialize Glad!");

		VOX_CORE_TRACE("OpenGL Info:");
		VOX_CORE_TRACE("\tVendor: {0}", (const char*)glGetString(GL_VENDOR));
		VOX_CORE_TRACE("\tRenderer: {0}", (const char*)glGetString(GL_RENDERER));
		VOX_CORE_TRACE("\tVersion: {0}",(const char*)glGetString(GL_VERSION));
	}
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}