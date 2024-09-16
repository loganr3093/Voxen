#pragma once
#include "Voxen.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include <filesystem>

namespace Voxen
{
	class VoxEditorLayer : public Layer
	{
	public:
		VoxEditorLayer();
		virtual ~VoxEditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e) override;

		virtual void OnUpdate(Timestep ts) override;

	private:
		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_SceneFilePath;
		EditorCamera m_EditorCamera;
	};
}