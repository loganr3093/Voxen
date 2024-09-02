#include "Demo.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

Demo::Demo()
	: Layer("Demo"), m_CameraController(1280.0f / 720.0f)
{
}

void Demo::OnAttach()
{
	VOX_PROFILE_FUNCTION();

	m_TestTexture = Voxen::Texture2D::Create("assets/textures/TestTexture.jpg");

	Voxen::FramebufferSpecification fbSpec;
	fbSpec.Width = 1280;
	fbSpec.Height = 720;
}

void Demo::OnDetach()
{
	VOX_PROFILE_FUNCTION();
}

void Demo::OnUpdate(Voxen::Timestep ts)
{
	VOX_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(ts);

	Voxen::Renderer2D::ResetStats();


	// Render
	Voxen::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Voxen::RenderCommand::Clear();

	Voxen::Renderer2D::BeginScene(m_CameraController.GetCamera());

	Voxen::Renderer2D::DrawQuad({ -5.0f, -5.0f, -0.1f }, { 10.0f, 10.0f }, m_TestTexture);

	Voxen::Renderer2D::EndScene();
}

void Demo::OnImGuiRender()
{
	VOX_PROFILE_FUNCTION();
}

void Demo::OnEvent(Voxen::Event& e)
{
	m_CameraController.OnEvent(e);
}