#include "voxpch.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

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
		VOX_PROFILE_FUNCTION();

		glCreateVertexArrays(1, &m_RendererID);
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}
	void OpenGLVertexArray::Bind() const
	{
		VOX_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
	}
	void OpenGLVertexArray::Unbind() const
	{
		VOX_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}
	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		VOX_PROFILE_FUNCTION();

		VOX_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout");
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Vector2:
			case ShaderDataType::Vector3:
			case ShaderDataType::Vector4:
			{
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribPointer(m_VertexBufferIndex,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)element.Offset);
				m_VertexBufferIndex++;
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::IVector2:
			case ShaderDataType::IVector3:
			case ShaderDataType::IVector4:
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribIPointer(m_VertexBufferIndex,
					element.GetComponentCount(),
					ShaderDataTypeToOpenGLBaseType(element.Type),
					layout.GetStride(),
					(const void*)element.Offset);
				m_VertexBufferIndex++;
				break;
			}
			case ShaderDataType::Matrix3:
			case ShaderDataType::Matrix4:
			{
				uint8_t count = element.GetComponentCount();
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribPointer(m_VertexBufferIndex,
						count,
						ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)(element.Offset + sizeof(float) * count * i));
					glVertexAttribDivisor(m_VertexBufferIndex, 1);
					m_VertexBufferIndex++;
				}
				break;
			}
			default:
				VOX_CORE_ASSERT(false, "Unknown ShaderDataType");
			}
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