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
	};

	Application* CreateApplication()
	{
		ApplicationSpecification spec;
		spec.Name = "Voxen-Editor";
		spec.WorkingDirectory = "./";

		//spec.RendererType = RendererType::Renderer2D;
		spec.RendererType = RendererType::VoxRenderer;

		return new VoxenEditor(spec);
	}
}