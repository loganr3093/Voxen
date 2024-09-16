#pragma once
#include "Voxen.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include "Voxen/Renderer/EditorCamera.h"

#include <filesystem>

namespace Voxen
{

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e) override;

		virtual void OnUpdate(Timestep ts) override;

		// Project
		bool OpenProject();
		bool OpenProject(const std::filesystem::path& filepath);
		void CreateProject(const std::filesystem::path& filepath, const std::string& projectName);
		void CreateEmptyProject();
		void UpdateCurrentProject();
		void SaveProject();
		void CloseProject();

		// Scene
		void NewScene(const std::string& sceneName = "NewScene");
		bool OpenScene();
		bool OpenScene(const std::filesystem::path& filepath, const bool checkAutoSave);
		void SaveScene();
		void SaveSceneAs();
		void AutoSaveScene();

	private:
		// Events
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void OnScenePlay();
		void OnSceneStop();

		// UI Panels
		void UI_Toolbar();

		// Popups
		void UI_ShowNewProjectPopup();
		void UI_ShowLoadAutoSavePopup();
		void UI_ShowNewScenePopup();

		// Utils
		void BuildProjectData();
		void RunBatchFileSolution(const std::filesystem::path& filepath);
		void OnDuplicateEntity();
		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& filepath);
		void HandleDragDrop();
	private:
		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_SceneFilePath;

		float m_TimeSinceLastSave = 0.0f;

		bool m_PrimaryCamera = true;

		Entity m_HoveredEntity;

		EditorCamera m_EditorCamera;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		Vector2 m_ViewportSize = { 0.0f, 0.0f };
		Vector2 m_ViewportBounds[2] = { { 0.0f, 0.0f },{ 0.0f, 0.0f } };

		struct GizmoSettings
		{
			int Type = -1;
			float PositionSnap = 00.5f;
			float RotationSnap = 22.5f;
		};
		GizmoSettings m_Gizmo;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};
		SceneState m_SceneState = SceneState::Edit;

		struct LoadAutoSavePopupData
		{
			std::string FilePath;
			std::string FilePathAuto;
		} m_LoadAutoSavePopupData;

		std::filesystem::path m_ProjectsPath = std::filesystem::current_path() / "Projects";

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		Scope<ContentBrowserPanel> m_ContentBrowserPanel;
	};
}