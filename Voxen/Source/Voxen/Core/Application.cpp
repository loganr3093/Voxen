#include "voxpch.h"
#include "Voxen/Core/Application.h"

#include "Voxen/Core/Log.h"
#include "Voxen/Core/Input.h"

#include "Voxen/Renderer/Renderer.h"

#include "Voxen/Editor/EditorResources.h"

#include "Voxen/Utilities/PlatformUtils.h"

#include "Voxen/Scripting/ScriptEngine.h"

namespace Voxen
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		VOX_PROFILE_FUNCTION();

		VOX_CORE_INFO("Initializing Application");
		VOX_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		// Set working directory here
		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		m_Window = Window::Create(WindowProps(m_Specification.Name));
		m_Window->SetEventCallback(VOX_BIND_EVENT_FN(Application::OnEvent));

		EditorResources::Init();

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		VOX_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		VOX_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::SubmitToMainThread(const std::function<void()>& function)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		m_MainThreadQueue.emplace_back(function);
	}

	void Application::OnEvent(Event& e)
	{
		VOX_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(VOX_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(VOX_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		VOX_PROFILE_FUNCTION();

		while (m_Running)
		{
			VOX_PROFILE_SCOPE("Run Loop");

			float time = Time::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			ExecuteMainThreadQueue();

			if (!m_Minimized)
			{
				{
					VOX_PROFILE_SCOPE("LayerStack OnUpdates");
					for (Layer* layer : m_LayerStack)
					{
						layer->OnUpdate(timestep);
					}
				}

				m_ImGuiLayer->Begin();
				{
					VOX_PROFILE_SCOPE("LayerStack OnImGuiRender");
					for (Layer* layer : m_LayerStack)
					{
						layer->OnImGuiRender();
					}
				}
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		VOX_CORE_INFO("Window Closing");
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		VOX_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::ExecuteMainThreadQueue()
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		for (auto& func : m_MainThreadQueue)
			func();

		m_MainThreadQueue.clear();
	}
}