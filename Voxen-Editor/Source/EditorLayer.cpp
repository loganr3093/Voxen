#include "EditorLayer.h"

#include "Voxen/Editor/EditorResources.h"

#include "Voxen/Scene/SceneSerializer.h"

#include "Voxen/Utilities/PlatformUtils.h"

#include "Voxen/Math/Math.h"

#include "Voxen/Scripting/ScriptEngine.h"
#include "Voxen/Scripting/ScriptBuilder.h"

#include "Voxen/VoxRenderer/VoxMemoryAllocator.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ImGuizmo.h>

namespace Voxen
{
	static std::string FormatBytes(size_t bytes)
	{
		const char* units[] = { "B", "KB", "MB", "GB", "TB" };
		size_t unitIndex = 0;
		double value = static_cast<double>(bytes);
		while (value >= 1024 && unitIndex < 4)
		{
			value /= 1024;
			unitIndex++;
		}
		char buffer[32];
		snprintf(buffer, sizeof(buffer), "%.2f %s", value, units[unitIndex]);
		return buffer;
	}

	static std::string FormatTime(float ms)
	{
		const char* units[] = { "ns", "μs", "ms", "s" };
		double value = ms;
		int unitIndex = 2;

		if (value < 1.0)
		{
			value *= 1000.0;
			unitIndex = 1;

			if (value < 1.0)
			{
				value *= 1000.0;
				unitIndex = 0;
			}
		}
		else if (value >= 1000.0)
		{
			value /= 1000.0;
			unitIndex = 3;
		}

		char buffer[32];
		snprintf(buffer, sizeof(buffer), "%.2f %s", value, units[unitIndex]);
		return buffer;
	}

	static bool s_CreateProjectPopupOpen = false;
	static bool s_NewScenePopupOpen = false;
	static bool s_InitializedMono = false;

	static void ReplaceToken(std::string& str, const char* token, const std::string& value)
	{
		size_t pos = 0;
		while ((pos = str.find(token, pos)) != std::string::npos)
		{
			str.replace(pos, strlen(token), value);
			pos += strlen(token);
		}
	}

	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		VOX_PROFILE_FUNCTION();

		FramebufferSpecification fbSpec;
		fbSpec.Attachments =
		{
			FramebufferTextureFormat::RGBA8,		// Color
			FramebufferTextureFormat::RED_INTEGER,	// Entity ID
			FramebufferTextureFormat::RGBA16F,		// Normal
			FramebufferTextureFormat::R32F,			// Depth
			FramebufferTextureFormat::Depth			// Depth stencil for raster
		};
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		m_Gizmo.Type = ImGuizmo::OPERATION::TRANSLATE;

		// TODO: Prompt the user to open a project or create a project in engine
		std::filesystem::path filepath = FileDialogs::OpenFile("Voxen Project (*.vproj)\0*.vproj\0");
		if (std::filesystem::exists(filepath) && Project::Load(filepath))
		{
			ScriptEngine::Init();

			auto activeProject = Project::GetActive();
			auto startScene = Project::GetActive()->GetConfig().StartScene;
			auto startScenePath = Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().StartScene);
			OpenScene(startScenePath, true);
			m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();
		}
		else
		{
			VOX_CORE_ASSERT(false, "Project must be loaded on startup");
		}

		m_EditorCamera = EditorCamera(45.0f, 1.778f, 0.1, 1000.0f);
	}

	void EditorLayer::OnDetach()
	{
		VOX_PROFILE_FUNCTION();

		EditorResources::Shutdown();
		ScriptEngine::Shutdown();
	}

	static Timestep timestep;
	static int shaderFrameCounter = 0;
	static std::string formattedVoxelTime = "0.00 ms";
	static std::string formattedAOTime = "0.00 ms";
	static std::string formattedBlurAOTime = "0.00 ms";
	static std::string formattedRenderQuadTime = "0.00 ms";
	static std::string formattedTotalTime = "0.00 ms";
	void EditorLayer::OnImGuiRender()
	{
		VOX_PROFILE_FUNCTION();

		// Note: Switch this to true to enable dockspace
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
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Create Project...", "Ctrl+Shift+N"))
					s_CreateProjectPopupOpen = true;

				if (ImGui::MenuItem("Open Project...", "Ctrl+Shift+O"))
					OpenProject();

				if (ImGui::MenuItem("Save Project"))
					SaveProject();

				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
					s_NewScenePopupOpen = true;

				if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
					OpenScene();

				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					SaveScene();

				if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scripts"))
			{
				if (ImGui::MenuItem("Reload Assemblies", "Ctrl+R"))
					ScriptEngine::ReloadAssembly();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		UI_ShowNewProjectPopup();
		UI_ShowNewScenePopup();

		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel->OnImGuiRender();

		ImGui::Begin("Statistics");

		ImGui::SeparatorText("Engine");
		ImGui::Indent();

		ImGui::Columns(2, "##engine_stats", false);
		ImGui::SetColumnWidth(0, 150.0f);

		if (timestep.GetMilliseconds() != 0)
		{
			ImGui::Text("Current FPS");
			ImGui::NextColumn();
			ImGui::Text("%.2f", 1000.0f / timestep.GetMilliseconds());
			ImGui::NextColumn();
		}

		ImGui::Text("Average FPS");
		ImGui::NextColumn();
		ImGui::Text("%.2f", m_AverageFPS);
		ImGui::NextColumn();

		ImGui::Text("Frame Drops (1s)");
		ImGui::NextColumn();
		ImGui::Text("%d", m_FrameDropsLastSecond);
		ImGui::NextColumn();

		ImGui::Columns(1);
		ImGui::Spacing();

		ImGui::Text("Hovered Entity: %s", m_HoveredEntity ? m_HoveredEntity.GetComponent<TagComponent>().Tag.c_str() : "None");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::SeparatorText("Renderer");
		ImGui::Indent();

		// TODO: Add Renderer::GetStats
		auto stats = VoxRenderer::GetStats(m_EditorScene);

		ImGui::Columns(2, "##count_stats", false);
		ImGui::SetColumnWidth(0, 150.0f);

		ImGui::Text("Octrees");
		ImGui::NextColumn();
		ImGui::Text("%d", stats.treeCount);
		ImGui::NextColumn();

		ImGui::Text("Nodes");
		ImGui::NextColumn();
		ImGui::Text("%d", stats.nodeCount);
		ImGui::NextColumn();

		ImGui::Text("Leaves");
		ImGui::NextColumn();
		ImGui::Text("%d", stats.leafCount);
		ImGui::NextColumn();

		ImGui::Columns(1);
		ImGui::Spacing();

		ImGui::Columns(2, "##memory_stats", false);
		ImGui::SetColumnWidth(0, 150.0f);

		ImGui::Text("Octree Memory");
		ImGui::NextColumn();
		ImGui::Text("%s", FormatBytes(stats.TreeSize()).c_str());
		ImGui::NextColumn();

		ImGui::Text("Node Memory");
		ImGui::NextColumn();
		ImGui::Text("%s", FormatBytes(stats.NodeSize()).c_str());
		ImGui::NextColumn();

		ImGui::Text("Leaf Memory");
		ImGui::NextColumn();
		ImGui::Text("%s", FormatBytes(stats.LeafSize()).c_str());
		ImGui::NextColumn();

		ImGui::Spacing();

		ImGui::Text("Total Memory");
		ImGui::NextColumn();
		ImGui::Text("%s", FormatBytes(stats.TotalMemoryUsage()).c_str());
		ImGui::NextColumn();
		ImGui::Columns(1);

		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		shaderFrameCounter++;
		if (shaderFrameCounter % 5 == 0)
		{
			formattedVoxelTime = FormatTime(stats.VoxelShaderTime);
			formattedAOTime = FormatTime(stats.AOShaderTime);
			formattedBlurAOTime = FormatTime(stats.BlurAOShaderTime);
			formattedRenderQuadTime = FormatTime(stats.RenderQuadTime);
			formattedTotalTime = FormatTime(stats.VoxelShaderTime + stats.AOShaderTime + stats.BlurAOShaderTime + stats.RenderQuadTime);
		}

		ImGui::SeparatorText("Shaders");
		ImGui::Indent();

		ImGui::Columns(2, "##shader_stats", false);
		ImGui::SetColumnWidth(0, 150.0f);

		// Voxel Shader Time
		ImGui::Text("Voxel Shader Time");
		ImGui::NextColumn();
		ImGui::Text("%s", formattedVoxelTime.c_str());
		ImGui::NextColumn();

		// AO Shader Time
		ImGui::Text("AO Shader Time");
		ImGui::NextColumn();
		ImGui::Text("%s", formattedAOTime.c_str());
		ImGui::NextColumn();

		// Blur AO Shader Time
		ImGui::Text("Blur AO Shader Time");
		ImGui::NextColumn();
		ImGui::Text("%s", formattedBlurAOTime.c_str());
		ImGui::NextColumn();

		// Render Quad Time
		ImGui::Text("Render Quad Time");
		ImGui::NextColumn();
		ImGui::Text("%s", formattedRenderQuadTime.c_str());
		ImGui::NextColumn();

		// Total Shader Time (you might want to leave an empty column or adjust as needed)
		ImGui::Text("Total Shader Time");
		ImGui::NextColumn();
		ImGui::Text("%s", formattedTotalTime.c_str());
		ImGui::NextColumn();

		ImGui::Columns(1);
		ImGui::Unindent();

		ImGui::End();

		ImGui::Begin("Render Settings");

		ImGui::SeparatorText("Lighting");
		ImGui::Indent();
		bool lightingEnabled = Voxen::VoxRenderer::IsLightingEnabled();
		if (ImGui::Checkbox("Enabled", &lightingEnabled))
		{
			Voxen::VoxRenderer::SetLightingEnabled(lightingEnabled);
		}

		ImGui::BeginDisabled(!lightingEnabled);
		float diffuseStrength = VoxRenderer::GetDiffuseStrength();
		if (ImGui::SliderFloat("Strength", &diffuseStrength, 0.0f, 3.0f))
		{
			// Update the diffuse strength
			VoxRenderer::SetDiffuseStrength(diffuseStrength);
		}

		glm::vec3 lightColor = VoxRenderer::GetLightColor();
		if (ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor)))
		{
			// Update the light color
			VoxRenderer::SetLightColor(lightColor);
		}
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::SeparatorText("Ambient Occlusion");
		ImGui::Indent();
		bool aoEnabled = Voxen::VoxRenderer::IsAOEnabled();
		if (ImGui::Checkbox("Enabled##", &aoEnabled))
		{
			Voxen::VoxRenderer::SetAOEnabled(aoEnabled);
		}

		ImGui::BeginDisabled(!aoEnabled);
		bool aoBlurEnabled = Voxen::VoxRenderer::IsAOBlurEnabled();
		if (ImGui::Checkbox("AO Blur", &aoBlurEnabled))
		{
			Voxen::VoxRenderer::SetAOBlurEnabled(aoBlurEnabled);
		}

		static float aoStrength = VoxRenderer::GetAOStrength();
		if (ImGui::SliderFloat("AO Strength", &aoStrength, 0.0f, 3.0f))
		{
			// Update the AO strength
			VoxRenderer::SetAOStrength(aoStrength);
		}
		ImGui::EndDisabled();
		ImGui::EndDisabled();
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::SeparatorText("Debug");
		ImGui::Indent();
		bool showNormalsEnabled = Voxen::VoxRenderer::IsShowNormalsEnabled();
		if (ImGui::Checkbox("Show Normals Enabled", &showNormalsEnabled))
		{
			Voxen::VoxRenderer::SetShowNormalsEnabled(showNormalsEnabled);
		}
		ImGui::Unindent();

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		HandleDragDrop();

		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (m_SceneState == SceneState::Edit && selectedEntity && m_Gizmo.Type != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			// Editor Camera
			const Matrix4& cameraProjection = m_EditorCamera.GetProjection();
			Matrix4 cameraView = m_EditorCamera.GetViewMatrix();

			// Entity Transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			Matrix4 transform = tc.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = m_Gizmo.Type == ImGuizmo::OPERATION::ROTATE ? m_Gizmo.RotationSnap : m_Gizmo.PositionSnap;
			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_Gizmo.Type, ImGuizmo::LOCAL, glm::value_ptr(transform), 
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				Vector3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				Vector3 deltaRotation = rotation - tc.Rotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		UI_Toolbar();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		if (m_SceneState == SceneState::Edit)
		{
			m_EditorCamera.OnEvent(e);
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(VOX_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(VOX_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		VOX_PROFILE_FUNCTION();
		timestep = ts;

		// Update FPS and frame drop metrics
		m_TimeAccumulator += ts.GetSeconds();
		m_FrameCounter++;

		// Check for frame drops (frame time exceeding threshold)
		if (ts.GetSeconds() > m_FrameDropThreshold)
		{
			m_FrameDropsCurrent++;
		}

		// Calculate average FPS and reset counters every second
		if (m_TimeAccumulator >= 0.5f)
		{
			m_AverageFPS = static_cast<float>(m_FrameCounter) / m_TimeAccumulator;
			m_FrameDropsLastSecond = m_FrameDropsCurrent;

			// Reset counters
			m_TimeAccumulator = 0.0f;
			m_FrameCounter = 0;
			m_FrameDropsCurrent = 0;
		}

		m_ActiveScene->OnViewportResize((uint32)m_ViewportSize.x, (uint32)m_ViewportSize.y);

		// Resize
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32)m_ViewportSize.x, (uint32)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}

		// Render
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();

		// Clear entity ID attachment
		m_Framebuffer->ClearAttachment(1, -1);

		switch (m_SceneState)
		{
			case SceneState::Edit:
			{
				m_EditorCamera.OnUpdate(ts);

				m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
				break;
			}
			case SceneState::Play:
			{
				m_ActiveScene->OnUpdateRuntime(ts);
				break;
			}
		}

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
			// VOX_CORE_TRACE("Hovered Entity ID: {0}", pixelData);

			if (pixelData >= 0)
			{
				auto candidate = Entity((entt::entity)pixelData, m_ActiveScene.get());
				if (candidate)
					m_HoveredEntity = candidate;
				else
					m_HoveredEntity = {};
			}
			else
			{
				m_HoveredEntity = {};
			}
		}

		m_Framebuffer->Unbind();
	}

	// Project

	bool EditorLayer::OpenProject()
	{
		std::filesystem::path filepath = FileDialogs::OpenFile("Voxen Project (*.vproj)\0*.vproj\0");
		if (filepath.empty())
			return false;

		return OpenProject(filepath);
	}

	bool EditorLayer::OpenProject(const std::filesystem::path& filepath)
	{
		if (Project::GetActive())
			CloseProject();

		if (!Project::Load(filepath))
			return false;

		std::filesystem::path batchFilePath = Project::GetProjectDirectory() / "Win-CreateScriptProjects.bat";
		VOX_CORE_ASSERT(std::filesystem::exists(batchFilePath));
		RunBatchFileSolution(batchFilePath);

		ScriptBuilder::BuildScriptAssembly();

		ScriptEngine::SetAppAssemblyFilepath(Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().ScriptModulePath));
		ScriptEngine::ReloadAssembly();

		auto activeProject = Project::GetActive();
		auto startScene = Project::GetActive()->GetConfig().StartScene;
		auto startScenePath = Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().StartScene);
		if (std::filesystem::exists(startScenePath))
			OpenScene(startScenePath, true);
		else
			NewScene();
		m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();

		return true;
	}

	void EditorLayer::CreateProject(const std::filesystem::path& filepath, const std::string& projectName)
	{
		if (!std::filesystem::exists(filepath))
			std::filesystem::create_directories(filepath);

		std::filesystem::copy("Resources/BaseProjectTemplate", filepath, std::filesystem::copy_options::recursive);
		std::filesystem::path voxenRootDirectory = std::filesystem::absolute("./Resources").parent_path().string();
		if (voxenRootDirectory.stem().string() == "Voxen-Editor")
			voxenRootDirectory = voxenRootDirectory.parent_path();

		std::string newProjectFileName = projectName + ".vproj";

		std::filesystem::path parentPath = std::filesystem::current_path().parent_path();
		std::filesystem::path scriptCoreLuaRelativePath = parentPath / "Voxen-ScriptCore" / "Build-ScriptCore.lua";
		std::string scriptCoreLuaAbsoluteDirectory = scriptCoreLuaRelativePath.string();
		ReplaceToken(scriptCoreLuaAbsoluteDirectory, "\\", "/");
		VOX_CORE_ASSERT(std::filesystem::exists(scriptCoreLuaAbsoluteDirectory));

		std::filesystem::path premake5path = parentPath / "Vendor" / "Binaries" / "Premake" / "Windows" / "premake5.exe";
		std::string premakeAbsoluteDirectory = premake5path.string();
		VOX_CORE_ASSERT(std::filesystem::exists(premakeAbsoluteDirectory));

		std::filesystem::path batFilePath = filepath / "Win-CreateScriptProjects.bat";

		// premake5.lua
		{
			std::ifstream stream(filepath / "premake5.lua");
			VOX_CORE_ASSERT(stream.is_open());
			std::stringstream ss;
			ss << stream.rdbuf();
			stream.close();

			std::string str = ss.str();
			ReplaceToken(str, "$PROJECT_NAME$", projectName);
			ReplaceToken(str, "$SCRIPT_CORE_LUA_RELATIVE_DIRECTORY$", scriptCoreLuaAbsoluteDirectory);

			std::ofstream ostream(filepath / "premake5.lua");
			ostream << str;
			ostream.close();
		}

		// Project File
		{
			std::ifstream stream(filepath / "Project.vproj");
			VOX_CORE_ASSERT(stream.is_open());
			std::stringstream ss;
			ss << stream.rdbuf();
			stream.close();

			std::string str = ss.str();
			ReplaceToken(str, "$PROJECT_NAME$", projectName);

			std::ofstream ostream(filepath / "Project.vproj");
			ostream << str;
			ostream.close();

			std::filesystem::rename(filepath / "Project.vproj", filepath / newProjectFileName);
		}

		// Bat File
		{
			std::ifstream stream(batFilePath);
			VOX_CORE_ASSERT(stream.is_open());
			std::stringstream ss;
			ss << stream.rdbuf();
			stream.close();

			std::string str = ss.str();
			ReplaceToken(str, "$PREMAKE_DIR$", premakeAbsoluteDirectory);

			std::ofstream ostream(batFilePath);
			ostream << str;
			ostream.close();
		}

		std::filesystem::create_directories(filepath / "Assets" / "Scripts" / "Source");
		std::filesystem::create_directories(filepath / "Assets" / "Scripts" / "Binaries");

		RunBatchFileSolution(batFilePath);
		OpenProject(filepath.string() + "/" + newProjectFileName);
		SaveProject();
	}

	void EditorLayer::CreateEmptyProject()
	{
	}

	void EditorLayer::UpdateCurrentProject()
	{
	}

	void EditorLayer::SaveProject()
	{
		// Project::SaveActive();
	}

	void EditorLayer::CloseProject()
	{
		SaveProject();

		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_SceneHierarchyPanel.SetContext(nullptr);
	}

	// Scene

	void EditorLayer::NewScene(const std::string& sceneName)
	{
		CloseScene();

		m_EditorScene = CreateRef<Scene>(sceneName);
		m_SceneHierarchyPanel.SetContext(m_EditorScene);

		m_EditorCamera = EditorCamera(45.0f, 1.778f, 0.1, 1000.0f);

		std::filesystem::path assetDirectory = Project::GetAssetDirectory();
		std::filesystem::path sceneDirectory = assetDirectory / "Scenes";

		if (std::filesystem::exists(sceneDirectory))
		{
			m_SceneFilePath = sceneDirectory;
		}
		else if (std::filesystem::exists(assetDirectory))
		{
			m_SceneFilePath = Project::GetAssetDirectory();
		}
		else
		{
			m_SceneFilePath = std::filesystem::current_path();
		}
	}

	bool EditorLayer::OpenScene()
	{
		std::filesystem::path filepath = FileDialogs::OpenFile("Voxen Scene (*.vscene)\0*.vscene\0");
		if (!filepath.empty())
			return OpenScene(filepath, true);
		return false;
	}

	bool EditorLayer::OpenScene(const std::filesystem::path& filepath, const bool checkAutoSave)
	{
		if (filepath.extension().string() != ".vscene")
		{
			VOX_WARN("Could not load {0} - not a scene file", filepath.filename().string());
			return false;
		}

		if (checkAutoSave)
		{
			auto filepathAuto = filepath;
			filepathAuto += ".auto";
			if (std::filesystem::exists(filepathAuto) && std::filesystem::last_write_time(filepathAuto) > std::filesystem::last_write_time(filepath))
			{
				m_LoadAutoSavePopupData.FilePath = filepath.string();
				m_LoadAutoSavePopupData.FilePathAuto = filepathAuto.string();
				UI_ShowLoadAutoSavePopup();
				return false;
			}
		}

		CloseScene();

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(filepath.string()))
		{
			m_EditorScene = newScene;
			m_SceneHierarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_SceneFilePath = filepath;
		}

		return true;
	}

	void EditorLayer::SaveScene()
	{
		if (!m_SceneFilePath.empty())
			SerializeScene(m_ActiveScene, m_SceneFilePath);
		else
			SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs()
	{
		std::filesystem::path filepath = FileDialogs::SaveFile("Voxen Scene (*.vscene)\0*.vscene\0");
		if (filepath.empty())
			return;
		SerializeScene(m_ActiveScene, filepath);
		m_SceneFilePath = filepath;
	}

	void EditorLayer::AutoSaveScene()
	{
		if (m_SceneFilePath.empty()) return;

		SceneSerializer serializer(m_EditorScene);
		serializer.Serialize(m_SceneFilePath.string() + ".auto");

		m_TimeSinceLastSave = 0.0f;
	}

	// Events

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
			case Key::N:
			{
				if (control)
					NewScene();

				break;
			}
			case Key::O:
			{
				if (control)
					OpenScene();

				break;
			}
			case Key::S:
			{
				if (control)
				{
					if (shift)
						SaveSceneAs();
					else
						SaveScene();
				}

				break;
			}

			// Scene Commands
			case Key::D:
			{
				if (control)
					OnDuplicateEntity();
				break;
			}
			case Key::F:
			{
				Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
				if (!selectedEntity)
					break;

				m_EditorCamera.Focus(m_ActiveScene->GetWorldSpaceTransform(selectedEntity).Translation);
				break;
			}
			case Key::Escape:
			{
				m_SceneHierarchyPanel.SetSelectedEntity({});
				break;
			}

			// Gizmos
			case Key::Q:
			{
				if (!ImGuizmo::IsUsing())
					m_Gizmo.Type = -1;
				break;
			}
			case Key::W:
			{
				if (!ImGuizmo::IsUsing())
					m_Gizmo.Type = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}
			case Key::E:
			{
				if (!ImGuizmo::IsUsing())
					m_Gizmo.Type = ImGuizmo::OPERATION::ROTATE;
				break;
			}
			case Key::R:
			{
				if (control)
				{
					ScriptEngine::ReloadAssembly();
				}
				else if(!ImGuizmo::IsUsing())
				{
					m_Gizmo.Type = ImGuizmo::OPERATION::SCALE;
				}
				break;
			}
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	void EditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		VOX_CORE_ASSERT(m_SceneState == SceneState::Play);

		if (m_SceneState == SceneState::Play)
			m_ActiveScene->OnRuntimeStop();

		m_SceneState = SceneState::Edit;

		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::CloseScene()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

	}

	// UI Panels

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.75f));
		auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.75f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 4.0f;

		Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? EditorResources::PlayIcon : EditorResources::StopIcon;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (m_SceneState == SceneState::Edit)
				OnScenePlay();
			else if (m_SceneState == SceneState::Play)
				OnSceneStop();
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	// Popups

	void EditorLayer::UI_ShowNewProjectPopup()
	{
		static char s_NewProjectName[255] = "";
		static char s_NewProjectFilepath[512] = "";

		if (s_CreateProjectPopupOpen)
		{
			ImGui::OpenPopup("New Project");
			s_CreateProjectPopupOpen = false;
		}

		if (ImGui::BeginPopupModal("New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::InputTextWithHint("##project_name_input", "Project Name", s_NewProjectName, sizeof(s_NewProjectName));

			ImGui::Spacing();

			ImGui::InputTextWithHint("##project_location", "Project Location", s_NewProjectFilepath, sizeof(s_NewProjectName));
			ImGui::SameLine();
			if (ImGui::Button("..."))
			{
				std::string result = FileDialogs::OpenFolder().string();
				memcpy(s_NewProjectFilepath, result.data(), result.length());
			}

			ImGui::Separator();

			if (ImGui::Button("Create"))
			{
				std::filesystem::path fullProjectPath;
				if (strlen(s_NewProjectFilepath) > 0)
					fullProjectPath = std::string(s_NewProjectFilepath) + "/" + std::string(s_NewProjectName);
				else
					fullProjectPath = m_ProjectsPath / std::string(s_NewProjectName);
				
				CreateProject(fullProjectPath, s_NewProjectName);
				memset(s_NewProjectFilepath, 0, sizeof(s_NewProjectFilepath));
				memset(s_NewProjectName, 0, sizeof(s_NewProjectName));
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void EditorLayer::UI_ShowLoadAutoSavePopup()
	{
		if (ImGui::BeginPopupModal("Load Auto-Save?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("The scene you are trying to load has a newer auto-save file.");
			ImGui::Text("Load the auto-saved scene instead?");
			ImGui::Spacing();
			if (ImGui::Button("Yes"))
			{
				OpenScene(m_LoadAutoSavePopupData.FilePathAuto, false);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No"))
			{
				OpenScene(m_LoadAutoSavePopupData.FilePath, false);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void EditorLayer::UI_ShowNewScenePopup()
	{
		static char s_NewSceneName[255] = "";

		if (s_NewScenePopupOpen)
		{
			ImGui::OpenPopup("New Scene");
			s_NewScenePopupOpen = false;  // Reset the flag after calling OpenPopup
		}

		if (ImGui::BeginPopupModal("New Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::InputTextWithHint("##scene_name_input", "Scene Name", s_NewSceneName, sizeof(s_NewSceneName));
			ImGui::Spacing();

			if (ImGui::Button("Create"))
			{
				std::string sceneName = s_NewSceneName;
				NewScene(sceneName);
				SerializeScene(m_ActiveScene, m_SceneFilePath / (sceneName + ".vscene"));
				memset(s_NewSceneName, 0, sizeof(s_NewSceneName));
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	// Utils

	void EditorLayer::BuildProjectData()
	{
	}

	void EditorLayer::RunBatchFileSolution(const std::filesystem::path& filepath)
	{
		std::filesystem::path originalPath = std::filesystem::current_path();

		std::filesystem::current_path(filepath.parent_path());

		std::string batchFilePath = filepath.string();
		std::replace(batchFilePath.begin(), batchFilePath.end(), '/', '\\');
		batchFilePath = "\"" + batchFilePath + "\"";

		system(batchFilePath.c_str());

		std::filesystem::current_path(originalPath);
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
			m_EditorScene->DuplicateEntity(selectedEntity);
	}

	void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	void EditorLayer::HandleDragDrop()
	{
		if (!ImGui::BeginDragDropTarget())
			return;

		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
		if (payload)
		{
			const wchar_t* pathData = (const wchar_t*)payload->Data;
			std::filesystem::path filePath = pathData;
			std::string extension = filePath.extension().string();

			if (extension == ".vscene")
			{
				OpenScene(filePath, true);
			}
			else if (extension == ".vox")
			{
				std::string fileName = filePath.stem().string();

				Entity model = m_EditorScene->CreateEntity(fileName);
				auto& vrc = model.AddComponent<VoxelRendererComponent>(filePath);
			}
		}
		ImGui::EndDragDropTarget();
	}
}
