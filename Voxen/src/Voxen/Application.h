#pragma once

#include "Core.h"

#include "Window.h"
#include "Voxen/LayerStack.h"
#include "Voxen/Events/Event.h"
#include "Voxen/Events/ApplicationEvent.h"

#include "Voxen/ImGui/ImGuiLayer.h"
#include "Voxen/Renderer/Shader.h"
#include "Voxen/Renderer/Buffer.h"
#include "Voxen/Renderer/VertexArray.h"

#include "Voxen/Renderer/OrthographicCamera.h"

namespace Voxen
{
	class VOXEN_API Application
	{
	public:
		Application();
		virtual ~Application() = default;

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

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::shared_ptr<VertexArray> m_SquareVA;
		std::shared_ptr<Shader> m_Shader2;

		OrthographicCamera m_Camera;

	};

	// To be defined by client
	Application* CreateApplication();
}

