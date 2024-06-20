#pragma once

#include "Voxen/Core.h"
#include "Voxen/Layer.h"

namespace Voxen
{
	class VOXEN_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);
	private:
		float m_Time = 0.0f;
	};
}