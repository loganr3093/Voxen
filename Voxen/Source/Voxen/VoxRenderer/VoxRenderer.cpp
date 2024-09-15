#include "voxpch.h"
#include "Voxen/VoxRenderer/VoxRenderer.h"

#include "Voxen/Renderer/ComputeShader.h"
#include "Voxen/Renderer/Shader.h"
#include "Voxen/Renderer/RenderCommand.h"
#include "Voxen/Editor/EditorResources.h"
#include "Voxen/Renderer/Texture.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Voxen
{
    struct VoxRendererData
    {
        GLuint FullscreenQuadVAO, FullscreenQuadVBO;
        Ref<ComputeShader> ComputeShader;
        Ref<TextureRW> RWTexture;
        Ref<Shader> FullscreenQuadShader;
    };

    static VoxRendererData s_Data;

    void VoxRenderer::Init()
    {
        VOX_PROFILE_FUNCTION();

        // Set up the fullscreen quad and shaders
        SetupFullscreenQuad();

        // Initialize the compute shader
        std::string computeShaderSource = R"(
        #version 430
        layout (local_size_x = 16, local_size_y = 16) in;
        layout (rgba32f, binding = 0) uniform image2D imgOutput;
        uniform vec4 color;
        void main() {
            ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
            imageStore(imgOutput, pixel_coords, color);
        }
        )";
        s_Data.ComputeShader = ComputeShader::Create("ColorComputeShader", computeShaderSource);

        // Create the read-write texture (800x600 for now)
        s_Data.RWTexture = TextureRW::Create(800, 600);

        // Fullscreen quad shader (for rendering the texture)
        s_Data.FullscreenQuadShader = Shader::Create("assets/shaders/FullscreenQuad.glsl");
    }

    void VoxRenderer::Shutdown()
    {
        VOX_PROFILE_FUNCTION();

        // Clean up OpenGL resources
        glDeleteVertexArrays(1, &s_Data.FullscreenQuadVAO);
        glDeleteBuffers(1, &s_Data.FullscreenQuadVBO);
    }

    void VoxRenderer::BeginScene(const EditorCamera& camera)
    {
        VOX_PROFILE_FUNCTION();

        // Scene setup if needed
    }

    void VoxRenderer::EndScene()
    {
        VOX_PROFILE_FUNCTION();

        // Finalize the scene rendering
    }

    void VoxRenderer::SetupFullscreenQuad()
    {
        // Fullscreen quad vertices
        float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &s_Data.FullscreenQuadVAO);
        glGenBuffers(1, &s_Data.FullscreenQuadVBO);

        glBindVertexArray(s_Data.FullscreenQuadVAO);

        glBindBuffer(GL_ARRAY_BUFFER, s_Data.FullscreenQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // Texture coordinate attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);
    }

    void VoxRenderer::RunComputeShader(const glm::vec4& color)
    {
        // Bind the compute shader
        s_Data.ComputeShader->Bind();

        // Set the color uniform
        s_Data.ComputeShader->SetVector4("color", color);

        // Bind the texture as an image for writing
        s_Data.RWTexture->Bind(0);

        // Dispatch the compute shader (assumed 800x600 texture)
        GLuint workgroupSizeX = (800 + 16 - 1) / 16;
        GLuint workgroupSizeY = (600 + 16 - 1) / 16;
        glDispatchCompute(workgroupSizeX, workgroupSizeY, 1);

        // Ensure all operations are complete
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // Unbind the compute shader
        s_Data.ComputeShader->Unbind();
    }

    void VoxRenderer::RenderFullscreenQuad(const glm::vec4& color)
    {
        // Run the compute shader to color the texture
        RunComputeShader(color);

        // Bind the fullscreen quad shader
        s_Data.FullscreenQuadShader->Bind();
        s_Data.FullscreenQuadShader->SetInt("screenTexture", 0);

        // Bind the RWTexture to texture unit 0
        s_Data.RWTexture->Bind(0);

        // Render the fullscreen quad
        glBindVertexArray(s_Data.FullscreenQuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Unbind the fullscreen quad shader
        s_Data.FullscreenQuadShader->Unbind();
    }
}
