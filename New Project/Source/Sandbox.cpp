#include <Voxen.h>
#include "Voxen/Core/EntryPoint.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

class Sandbox : public Voxen::Application
{
public:
	Sandbox()
	{
		PushLayer(new Sandbox2D());
	}
	~Sandbox()
	{

	}
};

Voxen::Application* Voxen::CreateApplication()
{
	return new Sandbox();
}