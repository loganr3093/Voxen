#include <Voxen.h>
#include <Voxen/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Voxen
{
	class VoxenEditor : public Application
	{
	public:
		VoxenEditor(const Voxen::ApplicationSpecification& specification)
			: Voxen::Application(specification)
		{
			PushLayer(new EditorLayer());
		}
		~VoxenEditor()
		{

		}
	};

	Application* CreateApplication()
	{
		ApplicationSpecification spec;
		spec.Name = "Voxen-Hub";
		spec.WorkingDirectory = "./";
		return new VoxenEditor(spec);
	}
}