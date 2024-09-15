#include "voxpch.h"
#include "Voxen/VoxRenderer/VoxRenderer.h"

#include "Voxen/Renderer/Shader.h"
#include "Voxen/Renderer/RenderCommand.h"

#include "Voxen/Editor/EditorResources.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Voxen
{
    struct VoxRendererData
    {
        // Drawing screen quad
        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<Shader> FullScreenQuadShader;
        
        Ref<Texture2D> ScreenTexture;

        // Raytrace
        Ref<Shader> RaytraceCompute;
    };

    static VoxRendererData s_Data;

    void VoxRenderer::Init()
    {
        VOX_PROFILE_FUNCTION();

        // Shaders
        s_Data.FullScreenQuadShader = Shader::Create(EditorResources::FullScreenQuadShader.string());
        s_Data.FullScreenQuadShader->SetInt("u_Texture", 0);
        s_Data.RaytraceCompute = Shader::Create(EditorResources::RaytraceShader.string());

        s_Data.ScreenTexture = Texture2D::Create(1280, 720);

        float fullScreenQuadVertices[] =
        {
            // positions            // texture Coords   // ID
            -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,         -1,     // Bottom Left
             0.5f, -0.5f, 0.0f,     1.0f, 0.0f,         -1,     // Bottom Right
             0.5f,  0.5f, 0.0f,     1.0f, 1.0f,         -1,     // Top    Right
            -0.5f,  0.5f, 0.0f,     0.0f, 1.0f,         -1,     // Top    Left
        };

        s_Data.QuadVertexBuffer = VertexBuffer::Create(fullScreenQuadVertices, sizeof(fullScreenQuadVertices));
        s_Data.QuadVertexBuffer->SetLayout({
            { ShaderDataType::Vector3, "a_Position" },
            { ShaderDataType::Vector2, "a_TexCoords" },
            { ShaderDataType::Int,	   "a_EntityID" }
            });

        s_Data.QuadVertexArray = VertexArray::Create();
        s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

        uint32 indices[] = { 0, 1, 2, 2, 3, 0 };
        Ref<IndexBuffer> quadIB = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32));
        s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
    }

    void VoxRenderer::Shutdown()
    {
    }

    void VoxRenderer::BeginScene(const EditorCamera& camera)
    {
    }

    void VoxRenderer::EndScene()
    {
    }
}