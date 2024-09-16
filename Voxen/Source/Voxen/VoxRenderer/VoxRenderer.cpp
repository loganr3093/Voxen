#include "voxpch.h"
#include "Voxen/VoxRenderer/VoxRenderer.h"

#include "Voxen/Renderer/ComputeShader.h"
#include "Voxen/Renderer/Texture.h"
#include "Voxen/Renderer/Shader.h"
#include "Voxen/Renderer/VertexArray.h"
#include "Voxen/Renderer/RenderCommand.h"

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
        Ref<TextureRW> RWTexture;  // Texture used by compute shader
        Ref<Shader> FullscreenQuadShader;  // Shader used for rendering the quad

        Ref<Texture2D> ScreenTexture;  // Texture to display the result of the compute shader

        EditorCamera Camera;
    };

    static VoxRendererData s_Data;

    void VoxRenderer::Init()
    {
        VOX_PROFILE_FUNCTION();

        // Set up the fullscreen quad vertices and indices
        SetupFullscreenQuad();

        // Initialize the compute shader
        std::string computeShaderSource = R"(
        #version 460
        layout (local_size_x = 16, local_size_y = 16) in;
        layout (rgba32f, binding = 0) uniform image2D imgOutput;
        uniform vec4 color;
        void main() {
            ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
            imageStore(imgOutput, pixel_coords, color);
        }
        )";
        //s_Data.ComputeShader = ComputeShader::Create("ColorComputeShader", computeShaderSource);
        s_Data.ComputeShader = ComputeShader::Create("Resources/Shaders/Raytrace.glsl");

        // Create the read-write texture
        s_Data.RWTexture = TextureRW::Create(1600, 900);

        // Fullscreen quad shader (for rendering the texture)
        s_Data.FullscreenQuadShader = Shader::Create("Resources/Shaders/FullscreenQuad.glsl");

        // Setup the screen texture (for rendering to the screen)
        s_Data.ScreenTexture = Texture2D::Create(1600, 900);
    }

    void VoxRenderer::Shutdown()
    {
        VOX_PROFILE_FUNCTION();
    }

    void VoxRenderer::BeginScene(const EditorCamera& camera)
    {
        VOX_PROFILE_FUNCTION();
        s_Data.Camera = camera;
    }

    void VoxRenderer::EndScene()
    {
        VOX_PROFILE_FUNCTION();
    }

    void VoxRenderer::SetupFullscreenQuad()
    {
        // Fullscreen quad vertices (positions, texture coords, and entity ID)
        float fullScreenQuadVertices[] =
        {
            // positions         // texture Coords   // ID
            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,         -1,  // Bottom Left
             0.5f, -0.5f, 0.0f,  1.0f, 0.0f,         -1,  // Bottom Right
             0.5f,  0.5f, 0.0f,  1.0f, 1.0f,         -1,  // Top Right
            -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,         -1   // Top Left
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

    void VoxRenderer::RunComputeShader(const Vector2& screenSize)
    {
        // Bind the compute shader
        s_Data.ComputeShader->Bind();

        // Set the uniforms
        s_Data.ComputeShader->SetVector2("u_ScreenSize", screenSize);
        s_Data.ComputeShader->SetMat4("u_CameraMatrix", s_Data.Camera.GetViewMatrix());

        // Bind the texture as an image for writing
        s_Data.RWTexture->BindImage(0);

        // Dispatch the compute shader (assuming 1280x720 texture)
        s_Data.ComputeShader->Dispatch(1600 / 16, 900 / 16, 1);

        // Ensure memory is synchronized before rendering
        s_Data.RWTexture->Unbind();
    }

    void VoxRenderer::RenderFullscreenQuad(const Vector2& screenSize)
    {
        // Run the compute shader to color the texture
        RunComputeShader(screenSize);

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