#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>

#include "Voxen/Scene/Components.h"

namespace Voxen
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}
	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}
	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		for (auto entityID : m_Context->m_Registry.view<entt::entity>())
		{
			Entity entity { entityID, m_Context.get() };
			auto& tc = entity.GetComponent<TagComponent>();
			ImGui::Text("%s", tc.Tag.c_str());
		}

		ImGui::End();
	}
}