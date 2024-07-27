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

	m_TestTexture = (Voxen::Texture2D::Create("assets/textures/TestTexture.png"));
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
	Voxen::Renderer2D::DrawRotatedQuad({ 1.0f, 1.0f }, { 0.5f, 0.5f }, rotation, {0.9f, 0.7f, 0.1f, 1.0f});

	Voxen::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	VOX_PROFILE_FUNCTION();
	// Note: Switch this to true to enable dockspace
	static bool dockingEnabled = true;
	if (dockingEnabled)
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistent = true;
		bool opt_fullscreen = opt_fullscreen_persistent;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Exit")) Voxen::Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Begin("Settings");

		auto stats = Voxen::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

		uint32_t textureID = m_TestTexture->GetRendererID();
		ImGui::Image((void*)textureID, ImVec2{ 256.0f, 256.0f });
		ImGui::End();

		ImGui::End();
	}
	else
	{
		ImGui::Begin("Settings");

		auto stats = Voxen::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

		uint32_t textureID = m_TestTexture->GetRendererID();
		ImGui::Image((void*)textureID, ImVec2{ 256.0f, 256.0f });
		ImGui::End();
	}
}

void Sandbox2D::OnEvent(Voxen::Event& e)
{
	m_CameraController.OnEvent(e);
}