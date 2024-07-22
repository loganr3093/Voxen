#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
	m_TestTexture = (Voxen::Texture2D::Create("assets/textures/TestTexture.png"));
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Voxen::Timestep ts)
{
	VOX_PROFILE_FUNCTION();

	// Update
	{
		VOX_PROFILE_SCOPE("CameraController::OnUpdate");
		m_CameraController.OnUpdate(ts);
	}

	// Render
	Voxen::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Voxen::RenderCommand::Clear();

	Voxen::Renderer2D::BeginScene(m_CameraController.GetCamera());

	Voxen::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 0.5f, 0.5f }, m_SquareColor);
	Voxen::Renderer2D::DrawQuad({ -1.1f, 0.5f }, { 1.0f, 0.5f }, { 0.3f, 0.2f, 0.8f, 1.0f });

	Voxen::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_TestTexture);

	Voxen::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	VOX_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::End();
}

void Sandbox2D::OnEvent(Voxen::Event& e)
{
	m_CameraController.OnEvent(e);
}