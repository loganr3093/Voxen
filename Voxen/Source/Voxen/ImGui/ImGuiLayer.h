#pragma once

#include "Voxen/Core/Layer.h"
#include "Voxen/Events/ApplicationEvent.h"
#include "Voxen/Events/KeyEvent.h"
#include "Voxen/Events/MouseEvent.h"

namespace Voxen
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
}