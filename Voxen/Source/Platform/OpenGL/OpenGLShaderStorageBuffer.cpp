#include "voxpch.h"
#include "Platform/OpenGL/OpenGLShaderStorageBuffer.h"

#include <glad/glad.h>

namespace Voxen
{
    OpenGLShaderStorageBuffer::OpenGLShaderStorageBuffer(const void* data, uint32_t size)
    {
        // Generate and bind the SSBO
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);

        // Allocate storage for the buffer and optionally provide initial data
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW);

        // Unbind the buffer after setup
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    OpenGLShaderStorageBuffer::~OpenGLShaderStorageBuffer()
    {
        // Clean up SSBO resources when no longer needed
        glDeleteBuffers(1, &m_RendererID);
    }

    void OpenGLShaderStorageBuffer::Bind(uint32_t binding) const
    {
        // Bind the SSBO to a binding point
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_RendererID);
    }

    void OpenGLShaderStorageBuffer::Unbind() const
    {
        // Unbind the SSBO
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    }
}