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
	VOX_PROFILE_FUNCTION();

	m_TestTexture = Voxen::Texture2D::Create("assets/textures/TestTexture.jpg");

	Voxen::FramebufferSpecification fbSpec;
	fbSpec.Width = 1280;
	fbSpec.Height = 720;
}

void Sandbox2D::OnDetach()
{
	VOX_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Voxen::Timestep ts)
{
	VOX_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(ts);

	Voxen::Renderer2D::ResetStats();


	// Render
	Voxen::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Voxen::RenderCommand::Clear();

	Voxen::Renderer2D::BeginScene(m_CameraController.GetCamera());

	Voxen::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 0.5f, 0.5f }, m_SquareColor);
	Voxen::Renderer2D::DrawQuad({ -1.1f, 0.5f }, { 1.0f, 0.5f }, { 0.3f, 0.2f, 0.8f, 1.0f });

	Voxen::Renderer2D::DrawQuad({ -5.0f, -5.0f, -0.1f }, { 10.0f, 10.0f }, m_TestTexture);

	static float rotation = 0;
	rotation += ts * 45.0f;

	Voxen::Renderer2D::DrawRotatedQuad({ 0.5f, -1.0f }, { 1.0f, 1.0f }, rotation, m_TestTexture);
	Voxen::Renderer2D::DrawRotatedQuad({ 1.0f, -1.0f, 0.1f }, { 0.5f, 0.5f }, rotation, { 0.9f, 0.4f, 0.2f, 0.5f });

	Voxen::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	VOX_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

	auto stats = Voxen::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Voxen::Event& e)
{
	m_CameraController.OnEvent(e);
}