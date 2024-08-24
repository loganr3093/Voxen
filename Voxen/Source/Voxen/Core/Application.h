#pragma once

#include "Voxen/Core/Core.h"
#include "Voxen/Core/Window.h"
#include "Voxen/Core/LayerStack.h"
#include "Voxen/Core/Timestep.h"

#include "Voxen/Events/Event.h"
#include "Voxen/Events/ApplicationEvent.h"

#include "Voxen/ImGui/ImGuiLayer.h"

#include "Voxen/Renderer/Shader.h"
#include "Voxen/Renderer/Buffer.h"
#include "Voxen/Renderer/VertexArray.h"
#include "Voxen/Renderer/OrthographicCamera.h"

namespace Voxen
{
	class Application
	{
	public:
		Application(const std::string& name = "Voxen");
		virtual ~Application() = default;

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		static Application* s_Instance;

		float m_LastFrameTime = 0.0f;
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
	};

	// To be defined by client
	Application* CreateApplication();
}

