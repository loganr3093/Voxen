#pragma once

#include "Voxen/Renderer/Shader.h"
#include "Voxen/Renderer/RenderCommand.h"
#include "Voxen/Renderer/Texture.h"
#include "Voxen/Renderer/EditorCamera.h"

#include "Voxen/Scene/Scene.h"

#include <glm/glm.hpp>

namespace Voxen
{
    class VoxRenderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void OnWindowResize(uint32 width, uint32 height);

        static void BeginScene(const Camera& camera, const Matrix4& cameraTransform);
        static void BeginEditorScene(const EditorCamera& camera);
        static void EndScene();

        static void RenderScene(Ref<Scene> scene);

        static void RenderFullscreenQuad();

    private:
        static void SetupFullscreenQuad();
        static void RunComputeShader();
    };
}
