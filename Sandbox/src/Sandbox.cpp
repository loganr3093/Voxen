#include <Voxen.h>

class ExampleLayer : public Voxen::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{
	}

	void OnUpdate() override
	{
	}
	virtual void OnImGuiRender() override
	{
	}
	void OnEvent(Voxen::Event& event) override
	{
	}
};

class Sandbox : public Voxen::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}
	~Sandbox()
	{

	}
};

Voxen::Application* Voxen::CreateApplication()
{
	return new Sandbox();
}