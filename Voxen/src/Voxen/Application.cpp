#include "voxpch.h"
#include "Application.h"

#include "Voxen/Log.h"

#include <glad/glad.h>

#include "Input.h"

namespace Voxen
{
	Application* Application::s_Instance = nullptr;

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::None:     return GL_NONE;
		case ShaderDataType::Bool:     return GL_BOOL;
		case ShaderDataType::Int:      return GL_INT;
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Vector2:  return GL_FLOAT;
		case ShaderDataType::Vector3:  return GL_FLOAT;
		case ShaderDataType::Vector4:  return GL_FLOAT;
		case ShaderDataType::IVector2: return GL_INT;
		case ShaderDataType::IVector3: return GL_INT;
		case ShaderDataType::IVector4: return GL_INT;
		case ShaderDataType::UVector2: return GL_UNSIGNED_INT;
		case ShaderDataType::UVector3: return GL_UNSIGNED_INT;
		case ShaderDataType::UVector4: return GL_UNSIGNED_INT;
		case ShaderDataType::DVector2: return GL_DOUBLE;
		case ShaderDataType::DVector3: return GL_DOUBLE;
		case ShaderDataType::DVector4: return GL_DOUBLE;
		case ShaderDataType::Matrix2:  return GL_FLOAT;
		case ShaderDataType::Matrix3:  return GL_FLOAT;
		case ShaderDataType::Matrix4:  return GL_FLOAT;
		case ShaderDataType::DMatrix2: return GL_DOUBLE;
		case ShaderDataType::DMatrix3: return GL_DOUBLE;
		case ShaderDataType::DMatrix4: return GL_DOUBLE;
		}
		VOX_CORE_ASSERT(false, "Unknown shader data type");
		return 0;
	}

	Application::Application()
	{
		VOX_CORE_INFO("Initializing Application");
		VOX_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(VOX_BIND_EVENT_FN(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		float vertices[3 * 7]
		{
			-0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f,
		};

		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		{
			BufferLayout layout =
			{
				{ ShaderDataType::Vector3, "a_Position" },
				{ ShaderDataType::Vector4, "a_Color" },
			};

			m_VertexBuffer->SetLayout(layout);
		}

		uint32 index = 0;
		const auto& layout = m_VertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer
			(
				index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset
			);
			index++;
		}


		uint32 indices[3] = { 0, 1, 2 };
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32)));

		std::string vertexSrc = R"(
			#version 460 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 460 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = v_Color;
			}
		)";

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(VOX_BIND_EVENT_FN(Application::OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			m_Shader->Bind();
			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate();
			}

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
			{
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		VOX_CORE_INFO("Window Closing");
		m_Running = false;
		return true;
	}
}