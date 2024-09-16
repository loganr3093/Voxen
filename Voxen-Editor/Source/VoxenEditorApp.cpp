#include <Voxen.h>
#include <Voxen/Core/EntryPoint.h>

#include "EditorLayer.h"
#include "ComputeLayer.h"

namespace Voxen
{
	class VoxenEditor : public Application
	{
	public:
		VoxenEditor(const Voxen::ApplicationSpecification& specification)
			: Voxen::Application(specification)
		{
			//PushLayer(new EditorLayer());
			PushLayer(new ComputeLayer());
		}
	};

	Application* CreateApplication()
	{
		ApplicationSpecification spec;
		spec.Name = "Voxen-Hub";
		spec.WorkingDirectory = "./";
		spec.RendererType = RendererType::VoxRenderer;

		return new VoxenEditor(spec);
	}
}