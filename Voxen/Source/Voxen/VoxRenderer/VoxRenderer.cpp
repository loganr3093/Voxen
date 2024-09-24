#include "voxpch.h"
#include "Voxen/VoxRenderer/VoxRenderer.h"

#include "Voxen/Renderer/ComputeShader.h"
#include "Voxen/Renderer/Texture.h"
#include "Voxen/Renderer/Shader.h"
#include "Voxen/Renderer/VertexArray.h"
#include "Voxen/Renderer/RenderCommand.h"
#include "Voxen/Renderer/ShaderStorageBuffer.h"

#include "Voxen/VoxRenderer/VoxelShape.h"
#include "Voxen/VoxRenderer/VoxMemoryAllocator.h"

#include "Voxen/Editor/EditorResources.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Voxen
{
    struct VoxRendererData
    {
        // Fullscreen quad setup
        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<IndexBuffer> QuadIndexBuffer;

        Ref<ComputeShader> ComputeShader;
        Ref<TextureRW> RWTexture;
        Ref<Shader> FullscreenQuadShader;

        Ref<Texture2D> ScreenTexture;

        Ref<ShaderStorageBuffer> VoxelShapeData;
        Ref<ShaderStorageBuffer> MaterialData;

        EditorCamera Camera;
    };

    static VoxRendererData s_Data;

    void VoxRenderer::Init()
    {
        VOX_PROFILE_FUNCTION();

        // Set up the fullscreen quad vertices and indices
        SetupFullscreenQuad();

        s_Data.RWTexture = TextureRW::Create(1600, 900);
        s_Data.ScreenTexture = Texture2D::Create(1600, 900);

        s_Data.ComputeShader = ComputeShader::Create(EditorResources::VoxelRendererShader);

        // Fullscreen quad shader (for rendering the texture)
        s_Data.FullscreenQuadShader = Shader::Create(EditorResources::FullScreenQuadShader);

        Ref<VoxelShape> shape = CreateRef<VoxelShape>();

        for (int z = 0; z < 16; ++z)
        {
            for (int y = 0; y < 16; ++y)
            {
                for (int x = 0; x < 16; ++x)
                {
                    shape->InsertVoxel({ x, y, z }, rand() % 255);
                }
            }
        }

        VoxMemoryAllocator::Allocate(shape);
        VoxMemoryAllocator::GenerateBuffers();

        std::vector<GPUVoxelShape> shapeBuffer = VoxMemoryAllocator::GetShapeBuffer();
        std::vector<uint32> voxelBuffer = VoxMemoryAllocator::GetVoxelBuffer();

        s_Data.VoxelShapeData = ShaderStorageBuffer::Create(shapeBuffer.data(), shapeBuffer.size() * sizeof(GPUVoxelShape));
        s_Data.MaterialData = ShaderStorageBuffer::Create(voxelBuffer.data(), voxelBuffer.size() * sizeof(uint32));
    }

    void VoxRenderer::Shutdown()
    {
        VOX_PROFILE_FUNCTION();
    }

    void VoxRenderer::OnWindowResize(uint32 width, uint32 height)
    {
        s_Data.RWTexture = TextureRW::Create(width, height);
        s_Data.ScreenTexture = Texture2D::Create(width, height);
    }

    void VoxRenderer::BeginScene(const Camera& camera, const Matrix4& cameraTransform)
    {
    }

    void VoxRenderer::BeginEditorScene(const EditorCamera& camera)
    {
        VOX_PROFILE_FUNCTION();
        s_Data.Camera = camera;
    }

    void VoxRenderer::EndScene()
    {
        VOX_PROFILE_FUNCTION();
    }

    void VoxRenderer::RenderScene(Ref<Scene> scene)
    {
        RenderFullscreenQuad();
    }

    void VoxRenderer::SetupFullscreenQuad()
    {
        // Fullscreen quad vertices (positions, texture coords, and entity ID)
        float fullScreenQuadVertices[] =
        {
            // positions         // texture Coords   // ID
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,         -1,  // Bottom Left
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f,         -1,  // Bottom Right
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f,         -1,  // Top Right
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,         -1   // Top Left
        };

        uint32 indices[] = { 0, 1, 2, 2, 3, 0 };  // Indices for two triangles forming a quad

        // Create the vertex buffer and vertex array
        s_Data.QuadVertexBuffer = VertexBuffer::Create(fullScreenQuadVertices, sizeof(fullScreenQuadVertices));
        s_Data.QuadVertexBuffer->SetLayout({
            { ShaderDataType::Vector3, "a_Position" },
            { ShaderDataType::Vector2, "a_TexCoords" },
            { ShaderDataType::Int,     "a_EntityID" }
            });

        s_Data.QuadVertexArray = VertexArray::Create();
        s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

        // Create the index buffer
        s_Data.QuadIndexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32));
        s_Data.QuadVertexArray->SetIndexBuffer(s_Data.QuadIndexBuffer);
    }

    void VoxRenderer::RunComputeShader()
    {
        // Bind the compute shader
        s_Data.ComputeShader->Bind();

        // Set the screen size uniform
        s_Data.ComputeShader->SetVector2("u_ScreenSize", { s_Data.RWTexture->GetWidth() , s_Data.RWTexture->GetHeight() });

        // Pass the view-projection matrix and camera position
        s_Data.ComputeShader->SetMat4("u_ViewProjectionMatrix", s_Data.Camera.GetViewProjection());
        s_Data.ComputeShader->SetVector3("u_CameraPosition", s_Data.Camera.GetPosition());

        // Bind the texture as an image for writing
        s_Data.RWTexture->BindImage(0);

        s_Data.VoxelShapeData->Bind(0);
        s_Data.MaterialData->Bind(1);

        // Dispatch the compute shader (assuming 1280x720 texture)
        int dispatchX = static_cast<int>(s_Data.RWTexture->GetWidth() / 16);
        int dispatchY = static_cast<int>(s_Data.RWTexture->GetHeight() / 16);
        s_Data.ComputeShader->Dispatch(dispatchX, dispatchY, 1);

        // Ensure memory is synchronized before rendering
        s_Data.RWTexture->Unbind();
    }

    void VoxRenderer::RenderFullscreenQuad()
    {
        // Run the compute shader to color the texture
        RunComputeShader();

        // Bind the fullscreen quad shader
        s_Data.FullscreenQuadShader->Bind();
        s_Data.FullscreenQuadShader->SetInt("u_Texture", 0);  // Bind texture to texture unit 0

        // Bind the read-write texture as the screen texture
        s_Data.RWTexture->Bind(0);

        // Bind the quad vertex array for rendering
        s_Data.QuadVertexArray->Bind();

        // Render the quad (6 vertices for 2 triangles)
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexBuffer->GetCount());

        // Unbind the shader and texture
        s_Data.FullscreenQuadShader->Unbind();
        s_Data.RWTexture->Unbind();
    }
}