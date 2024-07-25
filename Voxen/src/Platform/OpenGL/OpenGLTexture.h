#pragma once

#include "Voxen/Renderer/Texture.h"

#include <glad/glad.h>

namespace Voxen
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32 width, uint32 height);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D() override;

		virtual uint32 GetWidth() const override { return m_Width; }
		virtual uint32 GetHeight() const override { return m_Height; }

		virtual void SetData(void* data, uint32 size) override;

		virtual void Bind(uint32 slot = 0) const override;

		virtual bool operator==(const Texture& other) const override { return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID; }
	
	private:
		std::string m_Path;
		uint32 m_Width, m_Height;
		ID m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};
}

