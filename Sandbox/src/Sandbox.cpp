#include <Voxen.h>

class Sandbox : public Voxen::Application
{
public:
	Sandbox()
	{

	}
	~Sandbox()
	{

	}
};

Voxen::Application* Voxen::CreateApplication()
{
	return new Sandbox();
}