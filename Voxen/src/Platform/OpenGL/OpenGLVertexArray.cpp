#include "voxpch.h"
#include "OpenGLVertexArray.h"

#include "Voxen/Renderer/VertexArray.h"

#include <glad/glad.h>

namespace Voxen
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::None:     return GL_NONE;
			case ShaderDataType::Bool:     return GL_BOOL;
			case ShaderDataType::Int:      return GL_INT;
			case ShaderDataType::Float:    return GL_FLOAT;
			case ShaderDataType::Vector2:  return GL_FLOAT;
			case ShaderDataType::Vector3:  return GL_FLOAT;
			case ShaderDataType::Vector4:  return GL_FLOAT;
			case ShaderDataType::IVector2: return GL_INT;
			case ShaderDataType::IVector3: return GL_INT;
			case ShaderDataType::IVector4: return GL_INT;
			case ShaderDataType::UVector2: return GL_UNSIGNED_INT;
			case ShaderDataType::UVector3: return GL_UNSIGNED_INT;
			case ShaderDataType::UVector4: return GL_UNSIGNED_INT;
			case ShaderDataType::DVector2: return GL_DOUBLE;
			case ShaderDataType::DVector3: return GL_DOUBLE;
			case ShaderDataType::DVector4: return GL_DOUBLE;
			case ShaderDataType::Matrix2:  return GL_FLOAT;
			case ShaderDataType::Matrix3:  return GL_FLOAT;
			case ShaderDataType::Matrix4:  return GL_FLOAT;
			case ShaderDataType::DMatrix2: return GL_DOUBLE;
			case ShaderDataType::DMatrix3: return GL_DOUBLE;
			case ShaderDataType::DMatrix4: return GL_DOUBLE;
		}
		VOX_CORE_ASSERT(false, "Unknown shader data type");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}
	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}
	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}
	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		VOX_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout");
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribPointer
			(
				m_VertexBufferIndex,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset
			);
			m_VertexBufferIndex++;
		}
		m_VertexBuffers.push_back(vertexBuffer);
	}
	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}