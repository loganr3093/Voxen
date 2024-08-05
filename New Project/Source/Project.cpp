#include <Voxen.h>
#include "Voxen/Core/EntryPoint.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Demo.h"

class Project : public Voxen::Application
{
public:
	Project()
	{
		PushLayer(new Demo());
	}
	~Project()
	{

	}
};

Voxen::Application* Voxen::CreateApplication()
{
	return new Project();
}