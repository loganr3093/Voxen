#pragma once

#include "Voxen/Renderer/Framebuffer.h"

namespace Voxen
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
		virtual uint32 GetColorAttachmentRendererID() const { return m_ColorAttachment; }
	private:
		ID m_RendererID;
		uint32 m_ColorAttachment, m_DepthAttachment;
		FramebufferSpecification m_Specification;
	};
}