#include <Voxen.h>
#include "Voxen/Core/EntryPoint.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "EditorLayer.h"

namespace Voxen
{
	class VoxenEditor : public Application
	{
	public:
		VoxenEditor()
			: Application("Voxen Editor")
		{
			PushLayer(new EditorLayer());
		}
		~VoxenEditor()
		{

		}
	};

	Application* CreateApplication()
	{
		return new VoxenEditor();
	}
}