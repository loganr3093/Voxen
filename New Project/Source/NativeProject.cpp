#include <Voxen.h>
#include <Voxen/Core/EntryPoint.h>

#include "Demo.h"

class NativeProject : public Voxen::Application
{
public:
	NativeProject(const Voxen::ApplicationSpecification& specification)
		: Voxen::Application(specification)
	{
		PushLayer(new Demo());
	}
	~NativeProject()
	{

	}
};

Voxen::Application* Voxen::CreateApplication()
{
	ApplicationSpecification spec;
	spec.Name = "Demo";
	spec.WorkingDirectory = "../Voxen-Editor";
	return new NativeProject(spec);
}