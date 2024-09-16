#pragma once

#include "Voxen/Renderer/Shader.h"
#include "Voxen/Renderer/RenderCommand.h"
#include "Voxen/Renderer/Texture.h"
#include "Voxen/Renderer/EditorCamera.h"
#include <glm/glm.hpp>

namespace Voxen
{
    class VoxRenderer
    {
    public:
        static void Init();
        static void Shutdown();
        static void BeginScene(const EditorCamera& camera);
        static void EndScene();

        // New method to run the compute shader and render the quad
        static void RenderFullscreenQuad(const Vector2& screenSize);

    private:
        static void SetupFullscreenQuad();
        static void RunComputeShader(const Vector2& screenSize);
    };
}
