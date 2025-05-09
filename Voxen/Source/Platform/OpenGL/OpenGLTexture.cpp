#include "voxpch.h"
#include "Platform/OpenGL/OpenGLTexture.h"

#include <stb_image.h>

namespace Voxen
{
	namespace Utils
	{
		static GLenum TextureFormatToGLInternal(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::RGBA8:			return GL_RGBA8;
			case TextureFormat::RED_INTEGER:	return GL_R32I;
			case TextureFormat::RGBA16F:		return GL_RGBA16F;
			case TextureFormat::R32F:			return GL_R32F;
			default: VOX_CORE_ASSERT(false, "Unknown texture format!"); return 0;
			}
		}

		static GLenum TextureFormatToGLFormat(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::RGBA8:			return GL_RGBA;
			case TextureFormat::RED_INTEGER:	return GL_RED_INTEGER;
			case TextureFormat::RGBA16F:		return GL_RGBA;
			case TextureFormat::R32F:			return GL_RED;
			default: VOX_CORE_ASSERT(false, "Unknown texture format!"); return 0;
			}
		}

		static GLenum TextureFormatToGLType(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::RGBA8:			return GL_UNSIGNED_BYTE;
			case TextureFormat::RED_INTEGER:	return GL_INT;
			case TextureFormat::RGBA16F:		return GL_FLOAT;
			case TextureFormat::R32F:			return GL_FLOAT;
			default: VOX_CORE_ASSERT(false, "Unknown texture format!"); return 0;
			}
		}
	}

	// Texture 2D

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		VOX_PROFILE_FUNCTION();

		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path)
	{
		VOX_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);

		// Check for HDR file extension
		bool isHDR = path.find(".hdr") != std::string::npos ||
			path.find(".HDR") != std::string::npos;

		if (isHDR)
		{
			// Load HDR image
			float* data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
			VOX_CORE_ASSERT(data, "Failed to load HDR image!");
			m_Width = width;
			m_Height = height;

			// Set format based on channels
			if (channels == 4)
			{
				m_InternalFormat = GL_RGBA16F;
				m_DataFormat = GL_RGBA;
			}
			else
			{
				m_InternalFormat = GL_RGB16F;
				m_DataFormat = GL_RGB;
			}

			// Create and upload texture
			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height,
				m_DataFormat, GL_FLOAT, data);

			stbi_image_free(data);
		}
		else
		{
			// Load LDR image
			stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
			VOX_CORE_ASSERT(data, "Failed to load image!");
			m_Width = width;
			m_Height = height;

			// Set format based on channels
			if (channels == 4) {
				m_InternalFormat = GL_RGBA8;
				m_DataFormat = GL_RGBA;
			}
			else if (channels == 3) {
				m_InternalFormat = GL_RGB8;
				m_DataFormat = GL_RGB;
			}
			else {
				VOX_CORE_ASSERT(false, "Unsupported number of channels!");
			}

			// Create and upload texture
			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height,
				m_DataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}

		// Set common texture parameters
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		VOX_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		VOX_PROFILE_FUNCTION();

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		VOX_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		VOX_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTexture2D::Unbind(uint32_t slot) const
	{
		VOX_PROFILE_FUNCTION();

		glBindTextureUnit(slot, 0);  // Unbind texture by binding ID 0
	}

	// Texture RW

	OpenGLTextureRW::OpenGLTextureRW(uint32_t width, uint32_t height, GLenum internalFormat, GLenum format, GLenum type)
		: m_Width(width), m_Height(height), m_InternalFormat(internalFormat), m_Format(format), m_Type(type)
	{
		VOX_PROFILE_FUNCTION();

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	OpenGLTextureRW::OpenGLTextureRW(uint32_t width, uint32_t height, TextureFormat format)
		: m_Width(width), m_Height(height)
	{
		VOX_PROFILE_FUNCTION();
		m_InternalFormat = Utils::TextureFormatToGLInternal(format);
		m_Format = Utils::TextureFormatToGLFormat(format);
		m_Type = Utils::TextureFormatToGLType(format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	OpenGLTextureRW::~OpenGLTextureRW()
	{
		VOX_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTextureRW::SetData(void* data, uint32_t size)
	{
		VOX_PROFILE_FUNCTION();

		uint32_t bpp = m_Format == GL_RGBA ? 4 : 3;
		VOX_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must match texture size!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_Format, m_Type, data);
	}

	void OpenGLTextureRW::BindImage(uint32_t unit) const
	{
		VOX_PROFILE_FUNCTION();

		glBindImageTexture(unit, m_RendererID, 0, GL_FALSE, 0, GL_READ_WRITE, m_InternalFormat);
	}

	void OpenGLTextureRW::Bind(uint32_t slot) const
	{
		VOX_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTextureRW::Unbind(uint32_t slot) const
	{
		VOX_PROFILE_FUNCTION();

		glBindTextureUnit(slot, 0);
	}
}