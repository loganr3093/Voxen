#include <Voxen.h>
#include <Voxen/Core/EntryPoint.h>

#include "Demo.h"

class Project : public Voxen::Application
{
public:
	Project(const Voxen::ApplicationSpecification& specification)
		: Voxen::Application(specification)
	{
		PushLayer(new Demo());
	}
	~Project()
	{

	}
};

Voxen::Application* Voxen::CreateApplication()
{
	ApplicationSpecification spec;
	spec.Name = "Demo";
	spec.WorkingDirectory = "../Voxen-Editor";
	return new Project(spec);
}