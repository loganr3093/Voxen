#include <Voxen.h>

#include "Platform/OpenGL/OpenGLShader.h"
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Voxen::Layer
{
public:
	ExampleLayer() 
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
	{
		m_VertexArray.reset(Voxen::VertexArray::Create());

		float vertices[3 * 7]
		{
			-0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f,
		};

		Voxen::Ref<Voxen::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Voxen::VertexBuffer::Create(vertices, sizeof(vertices)));

		Voxen::BufferLayout layout =
		{
			{ Voxen::ShaderDataType::Vector3, "a_Position" },
			{ Voxen::ShaderDataType::Vector4, "a_Color" },
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32 indices[3] = { 0, 1, 2 };
		Voxen::Ref<Voxen::IndexBuffer> indexBuffer;
		indexBuffer.reset(Voxen::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Voxen::VertexArray::Create());

		float squareVertices[3 * 4] =
		{
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};

		Voxen::Ref<Voxen::VertexBuffer> squareVB;
		squareVB.reset(Voxen::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		Voxen::BufferLayout layout2 =
		{
			{ Voxen::ShaderDataType::Vector3, "a_Position" }
		};

		squareVB->SetLayout(layout2);
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Voxen::Ref<Voxen::IndexBuffer> squareIB;
		squareIB.reset(Voxen::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 460 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
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

		m_Shader.reset(Voxen::Shader::Create(vertexSrc, fragmentSrc));

		std::string flatColorShaderVertexSrc = R"(
			#version 460 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string flatColorShaderFragmentSrc = R"(
			#version 460 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0f);
			}
		)";

		m_Shader2.reset(Voxen::Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));
	}

	void OnUpdate(Voxen::Timestep ts) override
	{
		if (Voxen::Input::IsKeyPressed(VOX_KEY_LEFT))
		{
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		}
		else if (Voxen::Input::IsKeyPressed(VOX_KEY_RIGHT))
		{
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
		}

		if (Voxen::Input::IsKeyPressed(VOX_KEY_DOWN))
		{
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;
		}
		else if (Voxen::Input::IsKeyPressed(VOX_KEY_UP))
		{
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		}

		if (Voxen::Input::IsKeyPressed(VOX_KEY_A))
		{
			m_CameraRotation += m_CameraRotationSpeed * ts;
		}
		else if (Voxen::Input::IsKeyPressed(VOX_KEY_D))
		{
			m_CameraRotation -= m_CameraRotationSpeed * ts;
		}

		Voxen::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Voxen::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Voxen::Renderer::BeginScene(m_Camera);

		Voxen::Renderer::Submit(m_Shader, m_VertexArray);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<Voxen::OpenGLShader>(m_Shader2)->Bind();
		std::dynamic_pointer_cast<Voxen::OpenGLShader>(m_Shader2)->UploadUniformVector3("u_Color", m_SquareColor);

		for (int  j = 0; j < 10; j++)
		{
			for (int  i = 0; i < 10; i++)
			{
				glm::vec3 pos(i * 0.11f, j * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Voxen::Renderer::Submit(m_Shader2, m_SquareVA, transform);
			}
		}

		Voxen::Renderer::EndScene();
	}
	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}
	void OnEvent(Voxen::Event& event) override
	{
	}

private:
	Voxen::Ref<Voxen::Shader> m_Shader;
	Voxen::Ref<Voxen::VertexArray> m_VertexArray;

	Voxen::Ref<Voxen::VertexArray> m_SquareVA;
	Voxen::Ref<Voxen::Shader> m_Shader2;

	Voxen::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;
	float m_CameraRotation = 0;
	float m_CameraRotationSpeed = 180.0f;

	glm::vec3 m_SquareColor = { 0.2f, 0.8f, 0.2f };
};

class Sandbox : public Voxen::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}
	~Sandbox()
	{

	}
};

Voxen::Application* Voxen::CreateApplication()
{
	return new Sandbox();
}