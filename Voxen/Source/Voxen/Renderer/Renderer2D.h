#pragma once

#include "Voxen/Renderer/OrthographicCamera.h"
#include "Voxen/Renderer/Texture.h"
#include "Voxen/Renderer/Camera.h"

namespace Voxen
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const Matrix4& transform);
		static void BeginScene(const OrthographicCamera& camera); // TODO remove
		static void EndScene();

		static void Flush();

		// Primitives
		static void DrawQuad(const Vector2& position, const Vector2& size, const Vector4& color);
		static void DrawQuad(const Vector3& position, const Vector2& size, const Vector4& color);
		static void DrawQuad(const Vector3& position, const Vector3& size, const Vector4& color);
		static void DrawQuad(const Vector2& position, const Vector2& size, const Ref<Texture2D>& texture);
		static void DrawQuad(const Vector3& position, const Vector2& size, const Ref<Texture2D>& texture);
		static void DrawQuad(const Vector3& position, const Vector3& size, const Ref<Texture2D>& texture);

		static void DrawQuad(const Matrix4& transform, const Vector4& color);
		static void DrawQuad(const Matrix4& transform, const Ref<Texture2D>& texture);

		static void DrawRotatedQuad(const Vector2& position, const Vector2& size, float rotation, const Vector4& color);
		static void DrawRotatedQuad(const Vector3& position, const Vector2& size, float rotation, const Vector4& color);
		static void DrawRotatedQuad(const Vector2& position, const Vector2& size, float rotation, const Ref<Texture2D>& texture);
		static void DrawRotatedQuad(const Vector3& position, const Vector2& size, float rotation, const Ref<Texture2D>& texture);

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

