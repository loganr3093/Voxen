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

        static void SetAOEnabled(bool enabled);
        static bool IsAOEnabled();

        static void SetLightingEnabled(bool enabled);
        static bool IsLightingEnabled();

        static void SetShowNormalsEnabled(bool enabled);
        static bool IsShowNormalsEnabled();

		static void SetAOStrength(float strength);
        static float GetAOStrength();

        static void SetLightDirection(const glm::vec3& direction);
        static const glm::vec3& GetLightDirection();

        static void SetAmbientStrength(float strength);
        static float GetAmbientStrength();

        static void SetDiffuseStrength(float strength);
        static float GetDiffuseStrength();

        static void SetSpecularStrength(float strength);
        static float GetSpecularStrength();

        static void SetLightColor(const glm::vec3& color);
        static const glm::vec3& GetLightColor();

        struct Statistics
        {
            uint32 DrawCalls = 0;
            uint32 QuadCount = 0;

            uint32 GetTotalVertexCount() const { return QuadCount * 4; }
            uint32 GetTotalIndexCount() const { return QuadCount * 6; }
        };

    private:
        static void SetupQuad();
        static void RenderQuad();
        static void RunVoxelShader();
        static void RunAOShader();
    };
}
