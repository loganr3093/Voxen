#pragma once

#include "Voxen/Renderer/Renderer.h"

#include "Voxen/Renderer/Texture.h"
#include "Voxen/Renderer/Camera.h"
#include "Voxen/Renderer/EditorCamera.h"

#include "Voxen/Scene/Components.h"
#include "Voxen/Scene/Scene.h"

namespace Voxen
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32 width, uint32 height);

		static void BeginScene(const Camera& camera, const Matrix4& cameraTransform);
		static void BeginEditorScene(const EditorCamera& camera);
		static void EndScene();

		static void RenderScene(Ref<Scene> scene);

		static void Flush();

		// Primitives
		static void DrawQuad(const Vector2& position, const Vector2& size, const Vector4& color);
		static void DrawQuad(const Vector3& position, const Vector2& size, const Vector4& color);
		static void DrawQuad(const Vector3& position, const Vector3& size, const Vector4& color);
		static void DrawQuad(const Vector2& position, const Vector2& size, const Ref<Texture2D>& texture);
		static void DrawQuad(const Vector3& position, const Vector2& size, const Ref<Texture2D>& texture);
		static void DrawQuad(const Vector3& position, const Vector3& size, const Ref<Texture2D>& texture);

		static void DrawQuad(const Matrix4& transform, const Vector4& color, int entityID = -1);
		static void DrawQuad(const Matrix4& transform, const Ref<Texture2D>& texture, int entityID = -1);

		static void DrawRotatedQuad(const Vector2& position, const Vector2& size, float rotation, const Vector4& color);
		static void DrawRotatedQuad(const Vector3& position, const Vector2& size, float rotation, const Vector4& color);
		static void DrawRotatedQuad(const Vector2& position, const Vector2& size, float rotation, const Ref<Texture2D>& texture);
		static void DrawRotatedQuad(const Vector3& position, const Vector2& size, float rotation, const Ref<Texture2D>& texture);

		static void DrawSprite(const Matrix4& transform, SpriteRendererComponent& src, int entityID);

		struct Statistics
		{
			uint32 DrawCalls = 0;
			uint32 QuadCount = 0;

			uint32 GetTotalVertexCount() const { return QuadCount * 4; }
			uint32 GetTotalIndexCount() const { return QuadCount * 6; }
		};

		static Statistics GetStats();
		static void ResetStats();

	private:
		static void StartBatch();
		static void NextBatch();
	};
}

