#pragma once

#include "Voxen/Renderer/ShaderStorageBuffer.h"

namespace Voxen
{
    class OpenGLShaderStorageBuffer : public ShaderStorageBuffer
    {
    public:
        OpenGLShaderStorageBuffer(const void* data, uint32_t size);
        virtual ~OpenGLShaderStorageBuffer();

        virtual void Bind(uint32_t binding) const override;
        virtual void Unbind() const override;

        virtual void UpdateData(const void* data, uint32_t size) override;

    private:
        uint32_t m_RendererID;
    };
}
