#include "voxpch.h"
#include "Voxen/Renderer/Renderer.h"

#include "Voxen/Renderer/Renderer2D.h"
#include "Voxen/VoxRenderer/VoxRenderer.h"

namespace Voxen
{
	static RendererType s_RendererType;

	// Function pointers for renderer delegation
	static void (*InitFn)() = nullptr;
	static void (*ShutdownFn)() = nullptr;
	static void (*BeginSceneCameraFn)(const Camera&, const Matrix4&) = nullptr;
	static void (*BeginSceneEditorFn)(const EditorCamera&) = nullptr;
	static void (*EndSceneFn)() = nullptr;
	static void (*RenderSceneFn)(Ref<Scene> scene) = nullptr;

	void Renderer::Init(RendererType type)
	{
		VOX_PROFILE_FUNCTION();

		RenderCommand::Init();

		s_RendererType = type;
		switch (type)
		{
		case Voxen::RendererType::None:
			InitFn = nullptr;
			ShutdownFn = nullptr;
			BeginSceneCameraFn = nullptr;
			BeginSceneEditorFn = nullptr;
			EndSceneFn = nullptr;
			RenderSceneFn = nullptr;
			break;
		case Voxen::RendererType::Renderer2D:
			InitFn = Renderer2D::Init;
			ShutdownFn = Renderer2D::Shutdown;
			BeginSceneCameraFn = Renderer2D::BeginScene;
			BeginSceneEditorFn = Renderer2D::BeginEditorScene;
			EndSceneFn = Renderer2D::EndScene;
			RenderSceneFn = Renderer2D::RenderScene;
			break;
		case Voxen::RendererType::VoxRenderer:
			InitFn = VoxRenderer::Init;
			ShutdownFn = VoxRenderer::Shutdown;
			BeginSceneCameraFn = VoxRenderer::BeginScene;
			BeginSceneEditorFn = VoxRenderer::BeginEditorScene;
			EndSceneFn = VoxRenderer::EndScene;
			RenderSceneFn = VoxRenderer::RenderScene;
			break;
		default:
			VOX_CORE_ASSERT("Unknown Renderer");
			break;
		}

		if (InitFn)
			InitFn();
	}

	void Renderer::Shutdown()
	{
		if (ShutdownFn)
			ShutdownFn();
	}

	void Renderer::OnWindowResize(uint32 width, uint32 height)
	{
		RenderCommand::SetViewport(0, 0, width, height);

		if (s_RendererType == RendererType::Renderer2D)
			Renderer2D::OnWindowResize(width, height);
		else if (s_RendererType == RendererType::VoxRenderer)
			VoxRenderer::OnWindowResize(width, height);
	}

	void Renderer::BeginScene(const Camera& camera, const Matrix4& cameraTransform)
	{
		if (BeginSceneCameraFn)
			BeginSceneCameraFn(camera, cameraTransform);
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		if (BeginSceneEditorFn)
			BeginSceneEditorFn(camera);
	}

	void Renderer::EndScene()
	{
		if (EndSceneFn)
			EndSceneFn();
	}

	void Renderer::RenderScene(Ref<Scene> scene)
	{
		if (RenderSceneFn)
			RenderSceneFn(scene);
	}

	RendererType Renderer::GetType()
	{
		return s_RendererType;
	}
}