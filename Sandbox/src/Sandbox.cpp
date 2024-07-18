#include <Voxen.h>
#include "Voxen/Core/EntryPoint.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

class ExampleLayer : public Voxen::Layer
{
public:
	ExampleLayer() 
		: Layer("Example"), m_CameraController(1280.0f / 720.0f)
	{
		m_VertexArray = Voxen::VertexArray::Create();

		float vertices[3 * 7]
		{
			-0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f,
		};

		Voxen::Ref<Voxen::VertexBuffer> vertexBuffer;
		vertexBuffer = Voxen::VertexBuffer::Create(vertices, sizeof(vertices));

		Voxen::BufferLayout layout =
		{
			{ Voxen::ShaderDataType::Vector3, "a_Position" },
			{ Voxen::ShaderDataType::Vector4, "a_Color" },
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32 indices[3] = { 0, 1, 2 };
		Voxen::Ref<Voxen::IndexBuffer> indexBuffer;
		indexBuffer = Voxen::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA = Voxen::VertexArray::Create();

		float squareVertices[5 * 4] =
		{
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		};

		Voxen::Ref<Voxen::VertexBuffer> squareVB;
		squareVB = Voxen::VertexBuffer::Create(squareVertices, sizeof(squareVertices));

		Voxen::BufferLayout layout2 =
		{
			{ Voxen::ShaderDataType::Vector3, "a_Position" },
			{ Voxen::ShaderDataType::Vector2, "a_TexCoord" }
		};

		squareVB->SetLayout(layout2);
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Voxen::Ref<Voxen::IndexBuffer> squareIB;
		squareIB = Voxen::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32));
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

		m_Shader = Voxen::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

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

		m_FlatColorShader = Voxen::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = (Voxen::Texture2D::Create("assets/textures/TestTexture.png"));

		std::dynamic_pointer_cast<Voxen::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Voxen::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Voxen::Timestep ts) override
	{
		// Update
		m_CameraController.OnUpdate(ts);

		// Render
		Voxen::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Voxen::RenderCommand::Clear();

		Voxen::Renderer::BeginScene(m_CameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<Voxen::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Voxen::OpenGLShader>(m_FlatColorShader)->UploadUniformVector3("u_Color", m_SquareColor);

		for (int  j = 0; j < 10; j++)
		{
			for (int  i = 0; i < 10; i++)
			{
				glm::vec3 pos(i * 0.11f, j * 0.11f, -0.1f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Voxen::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();
		Voxen::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		Voxen::Renderer::EndScene();
	}
	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}
	void OnEvent(Voxen::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

private:
	Voxen::ShaderLibrary m_ShaderLibrary;
	Voxen::Ref<Voxen::Shader> m_Shader;
	Voxen::Ref<Voxen::VertexArray> m_VertexArray;

	Voxen::Ref<Voxen::Shader> m_FlatColorShader;
	Voxen::Ref<Voxen::VertexArray> m_SquareVA;

	Voxen::Ref<Voxen::Texture2D> m_Texture;

	Voxen::OrthographicCameraController m_CameraController;

	glm::vec3 m_SquareColor = { 0.2f, 0.2f, 0.8f };
};

class Sandbox : public Voxen::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}
	~Sandbox()
	{

	}
};

Voxen::Application* Voxen::CreateApplication()
{
	return new Sandbox();
}