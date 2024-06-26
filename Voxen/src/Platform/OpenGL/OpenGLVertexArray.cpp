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
	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		VOX_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout");
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		uint32 index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer
			(
				index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset
			);
			index++;
		}
		m_VertexBuffers.push_back(vertexBuffer);
	}
	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}