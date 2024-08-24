#pragma once

#include "Voxen/Core/Core.h"
#include "Voxen/Core/Log.h"
#include "Voxen/Scene/Scene.h"
#include "Voxen/Scene/Entity.h"


namespace Voxen
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }

	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};
}