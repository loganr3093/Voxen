#pragma once

#include "Voxen/Core/Timestep.h"

#include "Voxen/Renderer/OrthographicCamera.h"

#include "Voxen/Events/ApplicationEvent.h"
#include "Voxen/Events/MouseEvent.h"

namespace Voxen
{
	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		void OnResize(float width, float height);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		void SetZoomLevel(float level) { m_ZoomLevel = level; }
		float GetZoomLevel() { return m_ZoomLevel; }

		void SetRotationSpeed(float speed) { m_CameraRotationSpeed = speed; }
		float GetRotationSpeed() { return m_CameraRotationSpeed; }

		void SetTranslationSpeed(float speed) { m_CameraTranslationSpeed = speed; }
		float GetTranslationSpeed() { return m_CameraTranslationSpeed; }

		void SetPosition(Vector3 pos) { m_CameraPosition = pos; }
		Vector3 GetPosition() { return m_CameraPosition; }

		void SetRotation(float rot) { m_CameraRotation = rot; }
		float GetRotation() { return m_CameraRotation; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;

		bool m_Rotation = false;

		Vector3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraTranslationSpeed = 5.0f;

		// Rotation of the camera in degrees (counter-clockwise)
		float m_CameraRotation = 0.0f;
		float m_CameraRotationSpeed = 90.0f;
	};
}