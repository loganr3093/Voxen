#pragma once

#include "Voxen.h"

class Sandbox2D : public Voxen::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Voxen::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Voxen::Event& e) override;

private:
	Voxen::OrthographicCameraController m_CameraController;

	// Temporary
	Voxen::Ref<Voxen::VertexArray> m_SquareVA;
	Voxen::Ref<Voxen::Shader> m_FlatColorShader;

	Voxen::Ref<Voxen::Texture2D> m_TestTexture;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};
