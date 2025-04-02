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

        Ref<ComputeShader> VoxelShader;
		Ref<ComputeShader> SSAOShader;

        Ref<TextureRW> ColorRWTexture;
        Ref<TextureRW> EntityRWTexture;
        Ref<TextureRW> NormalRWTexture;
        Ref<TextureRW> DepthRWTexture;
        Ref<TextureRW> AORWTexture;

        Ref<Shader> QuadShader;

        Ref<ShaderStorageBuffer> TreeBuffer;
        Ref<ShaderStorageBuffer> NodeBuffer;
        Ref<ShaderStorageBuffer> LeafBuffer;
        Ref<ShaderStorageBuffer> PaletteBuffer;

		bool AOEnabled;
		float AOStrength;
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
        s_Data.NormalRWTexture = TextureRW::Create(1600, 900, TextureFormat::RGBA16F);
        s_Data.DepthRWTexture = TextureRW::Create(1600, 900, TextureFormat::R32F);
		s_Data.AORWTexture = TextureRW::Create(1600, 900, TextureFormat::R32F);

        s_Data.VoxelShader = ComputeShader::Create(EditorResources::VoxelRendererShader);
        s_Data.SSAOShader = ComputeShader::Create(EditorResources::SSAOShader);

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

		s_Data.AOEnabled = true;
		s_Data.AOStrength = 1.0f;
    }

    void VoxRenderer::Shutdown()
    {
        VOX_PROFILE_FUNCTION();
    }

    void VoxRenderer::OnWindowResize(uint32 width, uint32 height)
    {
        s_Data.ColorRWTexture = TextureRW::Create(width, height, TextureFormat::RGBA8);
        s_Data.EntityRWTexture = TextureRW::Create(width, height, TextureFormat::RED_INTEGER);
        s_Data.NormalRWTexture = TextureRW::Create(width, height, TextureFormat::RGBA16F);
        s_Data.DepthRWTexture = TextureRW::Create(width, height, TextureFormat::R32F);
		s_Data.AORWTexture = TextureRW::Create(width, height, TextureFormat::R32F);
    }

    void VoxRenderer::BeginScene(const Camera& camera, const Matrix4& cameraTransform)
    {
    }

    void VoxRenderer::BeginEditorScene(const EditorCamera& camera)
    {
        VOX_PROFILE_FUNCTION();
        s_Data.VoxelShader->Bind();
        s_Data.VoxelShader->SetMat4("u_ViewProjectionMatrix", camera.GetViewProjection());
        s_Data.VoxelShader->SetVector3("u_CameraPosition", camera.GetPosition());

		s_Data.SSAOShader->Bind();
        s_Data.SSAOShader->SetMat4("u_ViewProjectionMatrix", camera.GetViewProjection());
        s_Data.SSAOShader->SetMat4("u_InverseViewProjectionMatrix", glm::inverse(camera.GetViewProjection()));

        glm::vec3 pos = camera.GetPosition();
    }

    void VoxRenderer::EndScene()
    {
        VOX_PROFILE_FUNCTION();
    }

    void VoxRenderer::RenderScene(Ref<Scene> scene)
    {
        VOX_PROFILE_FUNCTION();

        // Run voxel shader for the first pass
        RunVoxelShader();

		// Run Ambient Occlusion shader
        if (s_Data.AOEnabled)
            RunAOShader();

        // Render quad
        RenderQuad();
    }

    void VoxRenderer::SetAOEnabled(bool enabled)
    {
        s_Data.AOEnabled = enabled;
    }

    bool VoxRenderer::IsAOEnabled()
    {
        return s_Data.AOEnabled;
    }

    void VoxRenderer::SetAOStrength(float strength)
    {
		s_Data.AOStrength = strength;
    }

    float VoxRenderer::GetAOStrength()
    {
        return s_Data.AOStrength;
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

    void VoxRenderer::RunVoxelShader()
    {
        VOX_PROFILE_FUNCTION();

        s_Data.VoxelShader->Bind();

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

        s_Data.VoxelShader->SetVector2("u_ScreenSize", { s_Data.ColorRWTexture->GetWidth() , s_Data.ColorRWTexture->GetHeight() });
        s_Data.VoxelShader->SetInt("u_NumShapes", VoxMemoryAllocator::Count());

        s_Data.ColorRWTexture->BindImage(0);
        s_Data.EntityRWTexture->BindImage(1);
        s_Data.NormalRWTexture->BindImage(2);
        s_Data.DepthRWTexture->BindImage(3);

        s_Data.TreeBuffer->Bind(0);
        s_Data.NodeBuffer->Bind(1);
        s_Data.LeafBuffer->Bind(2);
        s_Data.PaletteBuffer->Bind(3);

        // Dispatch the compute shader (assuming 1280x720 texture)
        int dispatchX = static_cast<int>(s_Data.ColorRWTexture->GetWidth() / 16);
        int dispatchY = static_cast<int>(s_Data.ColorRWTexture->GetHeight() / 16);
        s_Data.VoxelShader->Dispatch(dispatchX, dispatchY, 1);

        // Ensure memory is synchronized before rendering
        s_Data.ColorRWTexture->Unbind();
        s_Data.EntityRWTexture->Unbind();
    }

    void VoxRenderer::RunAOShader()
    {
        s_Data.SSAOShader->Bind();

        s_Data.SSAOShader->SetVector2("u_ScreenSize", { s_Data.ColorRWTexture->GetWidth(), s_Data.ColorRWTexture->GetHeight() });

        s_Data.SSAOShader->SetFloat("u_Radius", 0.5f);
        s_Data.SSAOShader->SetFloat("u_Bias", 0.025f);
        s_Data.SSAOShader->SetFloat("u_AOStrength", s_Data.AOStrength);

        s_Data.DepthRWTexture->Bind(0);
        s_Data.NormalRWTexture->Bind(1);
        s_Data.AORWTexture->BindImage(0);

        int dispatchX = static_cast<int>(s_Data.ColorRWTexture->GetWidth() / 16);
        int dispatchY = static_cast<int>(s_Data.ColorRWTexture->GetHeight() / 16);
        s_Data.SSAOShader->Dispatch(dispatchX, dispatchY, 1);
    }

    void VoxRenderer::RenderQuad()
    {
        VOX_PROFILE_FUNCTION();
        // Bind the fullscreen quad shader
        s_Data.QuadShader->Bind();
        s_Data.QuadShader->SetInt("u_ColorTexture", 0);
        s_Data.QuadShader->SetInt("u_EntityTexture", 1);
        s_Data.QuadShader->SetInt("u_AOEnabled", s_Data.AOEnabled ? 1 : 0);

        // Bind the read-write texture as the screen texture
        s_Data.ColorRWTexture->Bind(0);
        s_Data.EntityRWTexture->Bind(1);
        s_Data.NormalRWTexture->Bind(2);
		s_Data.DepthRWTexture->Bind(3);
        s_Data.AORWTexture->Bind(4);

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