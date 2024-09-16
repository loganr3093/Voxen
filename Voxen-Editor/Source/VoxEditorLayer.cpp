#include "VoxEditorLayer.h"

#include "Voxen/Editor/EditorResources.h"

#include "Voxen/Scene/SceneSerializer.h"

#include "Voxen/Utilities/PlatformUtils.h"

#include "Voxen/Math/Math.h"

#include "Voxen/Scripting/ScriptEngine.h"
#include "Voxen/Scripting/ScriptBuilder.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ImGuizmo.h>

namespace Voxen
{
	VoxEditorLayer::VoxEditorLayer()
		: Layer("VoxEditorLayer")
	{

	}

	void VoxEditorLayer::OnAttach()
	{
		VOX_PROFILE_FUNCTION();

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1600;
		fbSpec.Height = 900;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_EditorScene = CreateRef<Scene>();

		m_EditorCamera = EditorCamera(80.0f, 1.778f, 0.001, 1000.0f, { 0.0f, 0.0f, -10.0f });
	}

	void VoxEditorLayer::OnDetach()
	{
	}

	void VoxEditorLayer::OnImGuiRender()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistent = true;
		bool opt_fullscreen = opt_fullscreen_persistent;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

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

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ (float)Application::Get().GetWindow().GetWidth(), (float)Application::Get().GetWindow().GetHeight() }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void VoxEditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);
	}

	void VoxEditorLayer::OnUpdate(Timestep ts)
	{
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			Application::Get().GetWindow().GetWidth() > 0.0f && Application::Get().GetWindow().GetHeight() > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != Application::Get().GetWindow().GetWidth() || spec.Height != Application::Get().GetWindow().GetHeight()))
		{
			m_Framebuffer->Resize(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
			m_EditorCamera.SetViewportSize(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
		}

		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 1.0f, 0.1f, 1.0f, 1 });
		RenderCommand::Clear();

		m_Framebuffer->ClearAttachment(1, -1);

		m_EditorCamera.OnUpdate(ts);

		Renderer::BeginScene(m_EditorCamera);

		Renderer::RenderScene(m_EditorScene);

		Renderer::EndScene();

		m_Framebuffer->Unbind();
	}

}