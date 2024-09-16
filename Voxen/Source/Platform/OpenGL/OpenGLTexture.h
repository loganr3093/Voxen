#pragma once

#include "Voxen/Renderer/Texture.h"
#include <glad/glad.h>

namespace Voxen
{
    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(uint32_t width, uint32_t height);
        OpenGLTexture2D(const std::string& path);
        virtual ~OpenGLTexture2D();

        virtual uint32_t GetWidth() const override { return m_Width; }
        virtual uint32_t GetHeight() const override { return m_Height; }
        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        virtual void SetData(void* data, uint32_t size) override;

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void Unbind(uint32_t slot = 0) const override;

        virtual bool operator==(const Texture& other) const override
        {
            return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
        }
    private:
        std::string m_Path;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        GLenum m_InternalFormat, m_DataFormat;
    };

    class OpenGLTextureRW : public TextureRW
    {
    public:
        OpenGLTextureRW(uint32_t width, uint32_t height, GLenum internalFormat = GL_RGBA32F, GLenum format = GL_RGBA, GLenum type = GL_FLOAT);
        virtual ~OpenGLTextureRW();

        virtual uint32_t GetWidth() const override { return m_Width; }
        virtual uint32_t GetHeight() const override { return m_Height; }
        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        virtual void SetData(void* data, uint32_t size) override;

        virtual void BindImage(uint32_t unit) const override;

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void Unbind(uint32_t slot = 0) const override;

        virtual bool operator==(const Texture& other) const override
        {
            return m_RendererID == ((OpenGLTextureRW&)other).m_RendererID;
        }

    private:
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        GLenum m_InternalFormat, m_Format, m_Type;
    };
}
