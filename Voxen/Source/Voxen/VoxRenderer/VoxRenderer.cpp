#include "voxpch.h"
#include "Voxen/VoxRenderer/VoxRenderer.h"
#include "Voxen/VoxRenderer/SparseVoxelTree.h"
#include "Voxen/VoxRenderer/VoxMemoryAllocator.h"

#include "Voxen/Renderer/ComputeShader.h"
#include "Voxen/Renderer/Texture.h"
#include "Voxen/Renderer/Shader.h"
#include "Voxen/Renderer/VertexArray.h"
#include "Voxen/Renderer/RenderCommand.h"
#include "Voxen/Renderer/ShaderStorageBuffer.h"

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
        Ref<TextureRW> ColorRWTexture;
        Ref<TextureRW> EntityRWTexture;

        Ref<Shader> QuadShader;

        Ref<ShaderStorageBuffer> TreeBuffer;
        Ref<ShaderStorageBuffer> NodeBuffer;
        Ref<ShaderStorageBuffer> LeafBuffer;
        Ref<ShaderStorageBuffer> PaletteBuffer;
    };

    static VoxRendererData s_Data;

    void VoxRenderer::Init()
    {
        VOX_PROFILE_FUNCTION();

        // Set up the fullscreen quad vertices and indices
        SetupQuad();

        // Set up quad's textures
        s_Data.ColorRWTexture = TextureRW::Create(1600, 900, TextureFormat::RGBA8);
        s_Data.EntityRWTexture = TextureRW::Create(1600, 900, TextureFormat::RED_INTEGER);

        s_Data.ComputeShader = ComputeShader::Create(EditorResources::VoxelRendererShader);

        // Fullscreen quad shader (for rendering the texture)
        s_Data.QuadShader = Shader::Create(EditorResources::FullScreenQuadShader);

        // Set up the buffers
        std::vector<GPUSparseVoxelTree>		treeData = VoxMemoryAllocator::GetTreeData();
        std::vector<GPUSparseVoxelTreeNode> nodeData = VoxMemoryAllocator::GetNodeData();
        std::vector<uint32>					leafData = VoxMemoryAllocator::GetLeafData();
        std::vector<Vector4>			    paletteData = VoxMemoryAllocator::GetPaletteData();

        s_Data.TreeBuffer = ShaderStorageBuffer::Create(treeData.data(), treeData.size() * sizeof(GPUSparseVoxelTree));
        s_Data.NodeBuffer = ShaderStorageBuffer::Create(nodeData.data(), nodeData.size() * sizeof(GPUSparseVoxelTreeNode));
        s_Data.LeafBuffer = ShaderStorageBuffer::Create(leafData.data(), leafData.size() * sizeof(uint32));
        s_Data.PaletteBuffer = ShaderStorageBuffer::Create(paletteData.data(), paletteData.size() * sizeof(Vector4));
    }

    void VoxRenderer::Shutdown()
    {
        VOX_PROFILE_FUNCTION();
    }

    void VoxRenderer::OnWindowResize(uint32 width, uint32 height)
    {
        s_Data.ColorRWTexture = TextureRW::Create(width, height, TextureFormat::RGBA8);
        s_Data.EntityRWTexture = TextureRW::Create(width, height, TextureFormat::RED_INTEGER);
    }

    void VoxRenderer::BeginScene(const Camera& camera, const Matrix4& cameraTransform)
    {
    }

    void VoxRenderer::BeginEditorScene(const EditorCamera& camera)
    {
        VOX_PROFILE_FUNCTION();
        s_Data.ComputeShader->Bind();

        s_Data.ComputeShader->SetMat4("u_ViewProjectionMatrix", camera.GetViewProjection());
        s_Data.ComputeShader->SetVector3("u_CameraPosition", camera.GetPosition());

        glm::vec3 pos = camera.GetPosition();
    }

    void VoxRenderer::EndScene()
    {
        VOX_PROFILE_FUNCTION();
    }

    void VoxRenderer::RenderScene(Ref<Scene> scene)
    {
        VOX_PROFILE_FUNCTION();
        // Run the compute shader to color the texture
        RunComputeShader();

        // Render quad
        RenderQuad();
    }

    void VoxRenderer::SetupQuad()
    {
        VOX_PROFILE_FUNCTION();
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
        VOX_PROFILE_FUNCTION();

        s_Data.ComputeShader->Bind();

        if (VoxMemoryAllocator::IsStructureDirty())
        {
            auto treeData = VoxMemoryAllocator::GetTreeData();
            auto nodeData = VoxMemoryAllocator::GetNodeData();
            auto leafData = VoxMemoryAllocator::GetLeafData();
            auto paletteData = VoxMemoryAllocator::GetPaletteData();

            s_Data.TreeBuffer->UpdateData(treeData.data(), treeData.size() * sizeof(GPUSparseVoxelTree));
            s_Data.NodeBuffer->UpdateData(nodeData.data(), nodeData.size() * sizeof(GPUSparseVoxelTreeNode));
            s_Data.LeafBuffer->UpdateData(leafData.data(), leafData.size() * sizeof(uint32));
            s_Data.PaletteBuffer->UpdateData(paletteData.data(), paletteData.size() * sizeof(Vector4));
        }
        else if (VoxMemoryAllocator::IsDataDirty())
        {
            auto treeData = VoxMemoryAllocator::GetTreeData();
            s_Data.TreeBuffer->UpdateData(treeData.data(), treeData.size() * sizeof(GPUSparseVoxelTree));
        }

        s_Data.ComputeShader->SetVector2("u_ScreenSize", { s_Data.ColorRWTexture->GetWidth() , s_Data.ColorRWTexture->GetHeight() });
        s_Data.ComputeShader->SetInt("u_NumShapes", VoxMemoryAllocator::Count());

        s_Data.ColorRWTexture->BindImage(0);
        s_Data.EntityRWTexture->BindImage(1);

        s_Data.TreeBuffer->Bind(0);
        s_Data.NodeBuffer->Bind(1);
        s_Data.LeafBuffer->Bind(2);
        s_Data.PaletteBuffer->Bind(3);

        // Dispatch the compute shader (assuming 1280x720 texture)
        int dispatchX = static_cast<int>(s_Data.ColorRWTexture->GetWidth() / 16);
        int dispatchY = static_cast<int>(s_Data.ColorRWTexture->GetHeight() / 16);
        s_Data.ComputeShader->Dispatch(dispatchX, dispatchY, 1);

        // Ensure memory is synchronized before rendering
        s_Data.ColorRWTexture->Unbind();
        s_Data.EntityRWTexture->Unbind();
    }

    void VoxRenderer::RenderQuad()
    {
        VOX_PROFILE_FUNCTION();
        // Bind the fullscreen quad shader
        s_Data.QuadShader->Bind();
        s_Data.QuadShader->SetInt("u_ColorTexture", 0);
        s_Data.QuadShader->SetInt("u_EntityTexture", 1);

        // Bind the read-write texture as the screen texture
        s_Data.ColorRWTexture->Bind(0);
        s_Data.EntityRWTexture->Bind(1);

        // Bind the quad vertex array for rendering
        s_Data.QuadVertexArray->Bind();

        // Render the quad (6 vertices for 2 triangles)
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexBuffer->GetCount());

        // Unbind the shader and texture
        s_Data.QuadShader->Unbind();
        s_Data.ColorRWTexture->Unbind();
        s_Data.EntityRWTexture->Unbind();
    }
}