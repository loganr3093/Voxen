#pragma once

#include "Voxen/Types/Types.h"
#include "Voxen/Renderer/Buffer.h"

namespace Voxen
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32 size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;
	private:
		ID m_RendererID;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32* indices, uint32 count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual uint32 GetCount() const { return m_Count; }
	private:
		ID m_RendererID;
		uint32 m_Count;
	};
}