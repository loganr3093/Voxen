#pragma once

#include "Voxen/Renderer/Shader.h"
#include "Voxen/Renderer/RenderCommand.h"
#include "Voxen/Renderer/Texture.h"
#include "Voxen/Renderer/EditorCamera.h"

#include "Voxen/Scene/Scene.h"

#include "Voxen/VoxRenderer/SparseVoxelTree.h"

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

		static void SetAOBlurEnabled(bool enabled);
		static bool IsAOBlurEnabled();

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

        static void SetLightColor(const glm::vec3& color);
        static const glm::vec3& GetLightColor();

        struct Statistics
        {
			float VoxelShaderTime = 0.0f;
			float AOShaderTime = 0.0f;
			float BlurAOShaderTime = 0.0f;
			float RenderQuadTime = 0.0f;

			uint32 treeCount = 0;
			uint32 nodeCount = 0;
			uint32 leafCount = 0;

            uint32 TreeSize()
            {
				return sizeof(GPUSparseVoxelTree) * treeCount;
            }

            uint32 NodeSize()
            {
                return sizeof(GPUSparseVoxelTreeNode) * nodeCount;
            }

			uint32 LeafSize()
			{
				return sizeof(uint32) * leafCount;
			}

			uint32 TotalMemoryUsage()
			{
				return TreeSize() + NodeSize() + LeafSize();
			}
        };

        static Statistics GetStats();

    private:
        static void SetupQuad();
        static void RenderQuad();
        static void RunVoxelShader();
        static void RunAOShader();
		static void RunBlurAOShader();
    };
}
