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

#include "Voxen/Core/Timer.h"

#include "Voxen/Editor/EditorResources.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Voxen
{
	struct GPUPointLight
	{
		glm::vec3 Position;
		float Intensity;
		glm::vec3 Color;
		float Radius;
	};
    struct PointLight
    {
		GPUPointLight Light;
        UUID EntityID;
    };
    struct VoxRendererData
    {
        // Fullscreen quad setup
        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<IndexBuffer> QuadIndexBuffer;

        Ref<ComputeShader> VoxelShader;
		Ref<ComputeShader> SSAOShader;
        Ref<ComputeShader> BlurAOShader;

        Ref<TextureRW> ColorRWTexture;
        Ref<TextureRW> EntityRWTexture;
        Ref<TextureRW> NormalRWTexture;
        Ref<TextureRW> DepthRWTexture;
        Ref<TextureRW> AORWTexture;
        Ref<TextureRW> BlurredAORWTexture;

        Ref<Shader> QuadShader;

        Ref<ShaderStorageBuffer> TreeBuffer;
        Ref<ShaderStorageBuffer> NodeBuffer;
        Ref<ShaderStorageBuffer> LeafBuffer;
        Ref<ShaderStorageBuffer> PaletteBuffer;
        Ref<ShaderStorageBuffer> LightBuffer;

        bool AOEnabled;
		bool AOBlurEnabled;
        float AOStrength;

        bool LightingEnabled;
        float DiffuseStrength;
        float AmbientStrength;
        Vector3 LightDirection;
        Vector3 LightColor;

		bool ShowNormalsEnabled;

        std::vector<PointLight> PointLights;

        VoxRenderer::Statistics Stats;

		Ref<Scene> CurrentScene;
    };
    static VoxRendererData s_Data;

    VoxRenderer::Statistics VoxRenderer::GetStats(Ref<Scene> scene)
    {
        VOX_PROFILE_FUNCTION();

        auto alloc = scene->m_Allocator;
        s_Data.Stats.treeCount = alloc->Count();
        s_Data.Stats.nodeCount = static_cast<uint32_t>(alloc->GetNodeData().size());
        s_Data.Stats.leafCount = static_cast<uint32_t>(alloc->GetLeafData().size());

        return s_Data.Stats;
    }

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
        s_Data.BlurredAORWTexture = TextureRW::Create(1600, 900, TextureFormat::R32F);

        s_Data.VoxelShader = ComputeShader::Create(EditorResources::VoxelRendererShader);
        s_Data.SSAOShader = ComputeShader::Create(EditorResources::SSAOShader);
        s_Data.BlurAOShader = ComputeShader::Create(EditorResources::BlurAOShader);

        // Fullscreen quad shader (for rendering the texture)
        s_Data.QuadShader = Shader::Create(EditorResources::FullScreenQuadShader);

        // Set up the buffers
        s_Data.TreeBuffer = ShaderStorageBuffer::Create(nullptr, 0);
        s_Data.NodeBuffer = ShaderStorageBuffer::Create(nullptr, 0);
        s_Data.LeafBuffer = ShaderStorageBuffer::Create(nullptr, 0);
        s_Data.PaletteBuffer = ShaderStorageBuffer::Create(nullptr, 0);

        s_Data.LightBuffer = ShaderStorageBuffer::Create(s_Data.PointLights.data(), s_Data.PointLights.size() * sizeof(PointLight));

        s_Data.AOEnabled = true;
        s_Data.AOBlurEnabled = true;
		s_Data.AOStrength = 1.0f;

		s_Data.LightingEnabled = true;
		s_Data.DiffuseStrength = 1.0f;
		s_Data.AmbientStrength = 0.5f;
		s_Data.LightDirection = Vector3(0.2f, 0.65f, 0.4f);
		s_Data.LightColor = Vector3(1);

        s_Data.ShowNormalsEnabled = false;
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
        s_Data.BlurredAORWTexture = TextureRW::Create(width, height, TextureFormat::R32F);
    }

    void VoxRenderer::BeginScene(const Camera& camera, const Matrix4& cameraTransform)
    {
        VOX_PROFILE_FUNCTION();

        s_Data.VoxelShader->Bind();
        Matrix4 viewMatrix = glm::inverse(cameraTransform);
        Matrix4 viewProj = camera.GetProjection() * viewMatrix;
        s_Data.VoxelShader->SetMat4("u_ViewProjectionMatrix", viewProj);
        s_Data.VoxelShader->SetVector3("u_CameraPosition", glm::vec3(cameraTransform[3]));

        s_Data.SSAOShader->Bind();
        s_Data.SSAOShader->SetMat4("u_ViewProjectionMatrix", viewProj);
        s_Data.SSAOShader->SetMat4("u_InverseViewProjectionMatrix", glm::inverse(viewProj));

        s_Data.QuadShader->Bind();
        s_Data.QuadShader->SetMat4("u_InverseViewProjectionMatrix", glm::inverse(viewProj));
        s_Data.QuadShader->SetVector3("u_CameraPosition", glm::vec3(cameraTransform[3]));
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

        s_Data.QuadShader->Bind();
        s_Data.QuadShader->SetMat4("u_InverseViewProjectionMatrix", glm::inverse(camera.GetViewProjection()));
        s_Data.QuadShader->SetVector3("u_CameraPosition", camera.GetPosition());

        glm::vec3 pos = camera.GetPosition();
    }

    void VoxRenderer::EndScene()
    {
        VOX_PROFILE_FUNCTION();
    }

    void VoxRenderer::RenderScene(Ref<Scene> scene)
    {
        VOX_PROFILE_FUNCTION();

		if (s_Data.CurrentScene && s_Data.CurrentScene != scene)
		{
			s_Data.CurrentScene = scene;
            s_Data.CurrentScene->m_Allocator->MarkStructureDirty();
            s_Data.CurrentScene->m_Allocator->MarkDataDirty();
		}
        s_Data.CurrentScene = scene;

        // Run voxel shader for the first pass
		Timer timer;
		timer.Reset();
        RunVoxelShader(scene);
		s_Data.Stats.VoxelShaderTime = timer.Elapsed();

		// Run Ambient Occlusion shader
        if (s_Data.AOEnabled)
        {
            timer.Reset();
            RunAOShader();
            s_Data.Stats.AOShaderTime = timer.Elapsed();
        }
        else
        {
			s_Data.Stats.AOShaderTime = 0.0f;
        }
        if (s_Data.AOBlurEnabled)
        {
            timer.Reset();
            RunBlurAOShader();
            s_Data.Stats.BlurAOShaderTime = timer.Elapsed();
        }

        // Render quad
		timer.Reset();
        RenderQuad();
		s_Data.Stats.RenderQuadTime = timer.Elapsed();
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

    void VoxRenderer::RunVoxelShader(Ref<Scene> scene)
    {
        VOX_PROFILE_FUNCTION();

        s_Data.VoxelShader->Bind();

        auto alloc = scene->m_Allocator;

        if (alloc->IsStructureDirty())
        {
            auto treeData = alloc->GetTreeData();
            auto nodeData = alloc->GetNodeData();
            auto leafData = alloc->GetLeafData();
            auto paletteData = alloc->GetPaletteData();

            s_Data.TreeBuffer->UpdateData(treeData.data(), treeData.size() * sizeof(GPUSparseVoxelTree));
            s_Data.NodeBuffer->UpdateData(nodeData.data(), nodeData.size() * sizeof(GPUSparseVoxelTreeNode));
            s_Data.LeafBuffer->UpdateData(leafData.data(), leafData.size() * sizeof(uint32));
            s_Data.PaletteBuffer->UpdateData(paletteData.data(), paletteData.size() * sizeof(Vector4));
        }
        else if (alloc->IsDataDirty())
        {
            auto treeData = alloc->GetTreeData();
            s_Data.TreeBuffer->UpdateData(treeData.data(), treeData.size() * sizeof(GPUSparseVoxelTree));
        }

        s_Data.VoxelShader->SetVector2("u_ScreenSize", { s_Data.ColorRWTexture->GetWidth() , s_Data.ColorRWTexture->GetHeight() });
        s_Data.VoxelShader->SetInt("u_NumShapes", alloc->Count());

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

        s_Data.SSAOShader->SetFloat("u_Radius", 1.5f);
        s_Data.SSAOShader->SetFloat("u_Bias", 0.01f);
        s_Data.SSAOShader->SetFloat("u_AOStrength", s_Data.AOStrength);

        s_Data.DepthRWTexture->Bind(0);
        s_Data.NormalRWTexture->Bind(1);
        s_Data.AORWTexture->BindImage(0);

        int dispatchX = static_cast<int>(s_Data.ColorRWTexture->GetWidth() / 16);
        int dispatchY = static_cast<int>(s_Data.ColorRWTexture->GetHeight() / 16);
        s_Data.SSAOShader->Dispatch(dispatchX, dispatchY, 1);
    }

    void VoxRenderer::RunBlurAOShader()
    {
        VOX_PROFILE_FUNCTION();

        // Horizontal Pass
        s_Data.BlurAOShader->Bind();
        s_Data.BlurAOShader->SetInt("u_Direction", 0);
        s_Data.BlurAOShader->SetVector2("u_ScreenSize", { s_Data.AORWTexture->GetWidth(), s_Data.AORWTexture->GetHeight() });

        s_Data.AORWTexture->Bind(0); // Read from original AO
        s_Data.BlurredAORWTexture->BindImage(0); // Write to blurred AO

        int dispatchX = static_cast<int>((s_Data.AORWTexture->GetWidth() + 15) / 16);
        int dispatchY = static_cast<int>((s_Data.AORWTexture->GetHeight() + 15) / 16);
        s_Data.BlurAOShader->Dispatch(dispatchX, dispatchY, 1);

		RenderCommand::MemBarrier(MemoryBarrierBit::ShaderImageAccess);

        // Vertical Pass
        s_Data.BlurAOShader->Bind();
        s_Data.BlurAOShader->SetInt("u_Direction", 1);

        s_Data.BlurredAORWTexture->Bind(0); // Read from blurred AO
        s_Data.AORWTexture->BindImage(0); // Write back to original AO

        s_Data.BlurAOShader->Dispatch(dispatchX, dispatchY, 1);

        RenderCommand::MemBarrier(MemoryBarrierBit::ShaderImageAccess);
    }

    void VoxRenderer::RenderQuad()
    {
        VOX_PROFILE_FUNCTION();
        // Bind the fullscreen quad shader
        s_Data.QuadShader->Bind();
        s_Data.QuadShader->SetInt("u_ColorTexture", 0);
        s_Data.QuadShader->SetInt("u_EntityTexture", 1);

        s_Data.QuadShader->SetInt("u_AOEnabled", s_Data.AOEnabled ? 1 : 0);
        s_Data.QuadShader->SetInt("u_LightingEnabled", s_Data.LightingEnabled ? 1 : 0);
		s_Data.QuadShader->SetInt("u_ShowNormalsEnabled", s_Data.ShowNormalsEnabled ? 1 : 0);

        s_Data.QuadShader->SetVector3("u_SunLightDirection", s_Data.LightDirection);
        s_Data.QuadShader->SetVector3("u_SunLightColor", s_Data.LightColor);
        s_Data.QuadShader->SetFloat("u_AmbientStrength", s_Data.AmbientStrength);
        s_Data.QuadShader->SetFloat("u_DiffuseStrength", s_Data.DiffuseStrength);

        s_Data.LightBuffer->Bind(4); // Use binding point 4
		std::vector<GPUPointLight> pointLights(s_Data.PointLights.size());
        for (PointLight var : s_Data.PointLights)
        {
			pointLights.push_back(var.Light);
        }
        s_Data.LightBuffer->UpdateData(pointLights.data(), pointLights.size() * sizeof(GPUPointLight));
		s_Data.QuadShader->SetInt("u_NumPointLights", pointLights.size());

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

    void VoxRenderer::SetAOEnabled(bool enabled)
    {
        s_Data.AOEnabled = enabled;
    }

    bool VoxRenderer::IsAOEnabled()
    {
        return s_Data.AOEnabled;
    }

	void VoxRenderer::SetAOBlurEnabled(bool enabled)
	{
		s_Data.AOBlurEnabled = enabled;
	}

	bool VoxRenderer::IsAOBlurEnabled()
	{
		return s_Data.AOBlurEnabled;
	}

    void VoxRenderer::SetAOStrength(float strength)
    {
        s_Data.AOStrength = strength;
    }

    float VoxRenderer::GetAOStrength()
    {
        return s_Data.AOStrength;
    }

    void VoxRenderer::SetLightDirection(const glm::vec3& direction)
    {
        s_Data.LightDirection = glm::normalize(direction);
    }

    const glm::vec3& VoxRenderer::GetLightDirection()
    {
        return s_Data.LightDirection;
    }

    void VoxRenderer::SetAmbientStrength(float strength)
    {
        s_Data.AmbientStrength = strength;
    }

    float VoxRenderer::GetAmbientStrength()
    {
        return s_Data.AmbientStrength;
    }

	void VoxRenderer::SetDiffuseStrength(float strength)
	{
		s_Data.DiffuseStrength = strength;
	}

	float VoxRenderer::GetDiffuseStrength()
	{
		return s_Data.DiffuseStrength;
	}

	void VoxRenderer::SetLightingEnabled(bool enabled)
	{
		s_Data.LightingEnabled = enabled;
	}

	bool VoxRenderer::IsLightingEnabled()
	{
		return s_Data.LightingEnabled;
	}

	void VoxRenderer::SetShowNormalsEnabled(bool enabled)
	{
		s_Data.ShowNormalsEnabled = enabled;
	}

	bool VoxRenderer::IsShowNormalsEnabled()
	{
		return s_Data.ShowNormalsEnabled;
	}

	void VoxRenderer::SetLightColor(const glm::vec3& color)
	{
		s_Data.LightColor = color;
	}

	const glm::vec3& VoxRenderer::GetLightColor()
	{
		return s_Data.LightColor;
	}

    void VoxRenderer::AddPointLight(Entity entity, const PointLightComponent& component)
    {
        auto uuid = entity.GetUUID();
        // Check if the light already exists
        auto it = std::find_if(s_Data.PointLights.begin(), s_Data.PointLights.end(), [&](const PointLight& light)
        {
            return light.EntityID == uuid;
        });
        if (it != s_Data.PointLights.end())
        {
            return;
        }

        PointLight pointLight;
        pointLight.EntityID = uuid;
        pointLight.Light.Position = entity.GetComponent<TransformComponent>().Translation;
        pointLight.Light.Color = component.Color;
        pointLight.Light.Intensity = component.Intensity;
        pointLight.Light.Radius = component.Radius;

        s_Data.PointLights.push_back(pointLight);
    }

    void VoxRenderer::RemovePointLight(Entity entity)
    {
        auto uuid = entity.GetUUID();
        auto& lights = s_Data.PointLights;
        lights.erase(std::remove_if(lights.begin(), lights.end(), [&](const PointLight& light)
        {
        return light.EntityID == uuid;
        }), lights.end());
    }

    void VoxRenderer::UpdatePointLight(Entity entity, const glm::vec3& position, const glm::vec3& color, float intensity, float radius)
    {
        auto uuid = entity.GetUUID();
        auto it = std::find_if(s_Data.PointLights.begin(), s_Data.PointLights.end(), [&](const PointLight& light)
        {
            return light.EntityID == uuid;
        });
        if (it != s_Data.PointLights.end())
        {
            it->Light.Position = position;
            it->Light.Color = color;
            it->Light.Intensity = intensity;
            it->Light.Radius = radius;
        }
    }
}