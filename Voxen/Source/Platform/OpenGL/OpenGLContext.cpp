#include "voxpch.h"
#include "Platform/OpenGL/OpenGLContext.h"

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
		VOX_PROFILE_FUNCTION();

		VOX_CORE_INFO("Initializing OpenGLContext");
		glfwMakeContextCurrent(m_WindowHandle);
		int32 status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		VOX_CORE_ASSERT(status, "Failed to initialize Glad!");

		VOX_CORE_TRACE("OpenGL Info:");
		VOX_CORE_TRACE("\tVendor: {0}", (const char*)glGetString(GL_VENDOR));
		VOX_CORE_TRACE("\tRenderer: {0}", (const char*)glGetString(GL_RENDERER));
		VOX_CORE_TRACE("\tVersion: {0}",(const char*)glGetString(GL_VERSION));

	#ifdef VOX_ENABLE_ASSERTS
		int versionMajor, versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		VOX_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 6), "Voxen requires at least OpenGL version 4.6")
	#endif
	}
	void OpenGLContext::SwapBuffers()
	{
		VOX_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}
}