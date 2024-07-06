#pragma once

#include "Voxen/Renderer/Texture.h"

namespace Voxen
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D() override;

		virtual uint32 GetWidth() const override { return m_Width; }
		virtual uint32 GetHeight() const override { return m_Height; }

		virtual void Bind(uint32 slot = 0) const override;
	
	private:
		std::string m_Path;
		uint32 m_Width, m_Height;
		ID m_RendererID;
	};
}

