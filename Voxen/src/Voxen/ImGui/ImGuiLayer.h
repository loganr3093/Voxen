#pragma once

#include "Voxen/Core/Layer.h"
#include "Voxen/Events/ApplicationEvent.h"
#include "Voxen/Events/KeyEvent.h"
#include "Voxen/Events/MouseEvent.h"

namespace Voxen
{
	class VOXEN_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};
}