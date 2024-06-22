#include <Voxen.h>

class ExampleLayer : public Voxen::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{
	}

	void OnUpdate() override
	{
		//VOX_INFO("ExampleLayer::Update");
	}
	void OnEvent(Voxen::Event& event) override
	{
		//VOX_TRACE("{0}", event.ToString());
	}
};

class Sandbox : public Voxen::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Voxen::ImGuiLayer());
	}
	~Sandbox()
	{

	}
};

Voxen::Application* Voxen::CreateApplication()
{
	return new Sandbox();
}