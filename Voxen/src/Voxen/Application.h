#pragma once

#include "Core.h"

#include "Window.h"
#include "Voxen/LayerStack.h"
#include "Voxen/Events/Event.h"
#include "Voxen/Events/ApplicationEvent.h"

#include "Voxen/ImGui/ImGuiLayer.h"
#include "Voxen/Renderer/Shader.h"

namespace Voxen
{
	class VOXEN_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
	private:
		static Application* s_Instance;

		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		uint32 m_VertexArray, m_VertexBuffer, m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
	};

	// To be defined by client
	Application* CreateApplication();
}

