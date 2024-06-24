#pragma once

#include "Voxen/Layer.h"
#include "Voxen/Events/ApplicationEvent.h"
#include "Voxen/Events/KeyEvent.h"
#include "Voxen/Events/MouseEvent.h"

namespace Voxen
{
	class VOXEN_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};
}