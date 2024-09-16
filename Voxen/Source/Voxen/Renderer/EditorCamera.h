#pragma once

#include "Voxen/Core/Timestep.h"

#include "Voxen/Events/Event.h"
#include "Voxen/Events/MouseEvent.h"

#include "Voxen/Renderer/Camera.h"

#include <glm/glm.hpp>


namespace Voxen
{
	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip, const Vector3& position = { 0.0f, 0.0f, 0.0f});

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		void Focus(const Vector3& focusPoint);

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

		const Matrix4& GetViewMatrix() const { return m_ViewMatrix; }
		Matrix4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		Vector3 GetUpDirection() const;
		Vector3 GetRightDirection() const;
		Vector3 GetForwardDirection() const;
		const Vector3& GetPosition() const { return m_Position; }
		Quaternion GetOrientation() const;
		Matrix4 GetProjectionMatrix() const { return m_Projection; }

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }

		float GetFOV() const { return m_FOV; }
		float GetAspectRatio() const { return m_AspectRatio; }
		float GetNearClip() const { return m_NearClip; }
		float GetFarClip() const { return m_FarClip; }
	private:
		void UpdateProjection();
		void UpdateView();

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const Vector2& delta);
		void MouseRotate(const Vector2& delta);
		void MouseZoom(float delta);

		Vector3 CalculatePosition() const;

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;
	private:
		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

		glm::mat4 m_ViewMatrix;
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		float m_Distance = 10.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		float m_MinFocusDistance{ 100.0f };

		float m_ViewportWidth = 1280, m_ViewportHeight = 720;
	};
}