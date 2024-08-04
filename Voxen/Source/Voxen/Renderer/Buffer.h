#pragma once

namespace Voxen
{
	enum class ShaderDataType : uint8
	{
		None = 0,
		Bool,
		Int,
		Float,
		Vector2,	Vector3,	Vector4,
		IVector2,	IVector3,	IVector4,
		UVector2,	UVector3,	UVector4,
		DVector2,	DVector3,	DVector4,
		Matrix2,	Matrix3,	Matrix4,
		DMatrix2,	DMatrix3,	DMatrix4,
	};

	static uint32 ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::None:     return 0;
		case ShaderDataType::Bool:     return 1;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Vector2:  return 4 * 2;
		case ShaderDataType::Vector3:  return 4 * 3;
		case ShaderDataType::Vector4:  return 4 * 4;
		case ShaderDataType::IVector2: return 4 * 2;
		case ShaderDataType::IVector3: return 4 * 3;
		case ShaderDataType::IVector4: return 4 * 4;
		case ShaderDataType::UVector2: return 4 * 2;
		case ShaderDataType::UVector3: return 4 * 3;
		case ShaderDataType::UVector4: return 4 * 4;
		case ShaderDataType::DVector2: return 8 * 2;
		case ShaderDataType::DVector3: return 8 * 3;
		case ShaderDataType::DVector4: return 8 * 4;
		case ShaderDataType::Matrix2:  return 4 * 2 * 2;
		case ShaderDataType::Matrix3:  return 4 * 3 * 3;
		case ShaderDataType::Matrix4:  return 4 * 4 * 4;
		case ShaderDataType::DMatrix2: return 8 * 2 * 2;
		case ShaderDataType::DMatrix3: return 8 * 3 * 3;
		case ShaderDataType::DMatrix4: return 8 * 4 * 4;
		}
		VOX_CORE_ASSERT(false, "Unknown shader data type");
		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32 Size;
		uint32 Offset;
		bool Normalized;

		BufferElement() = default;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {}

		uint32 GetComponentCount() const
		{
			switch (Type)
			{
			case ShaderDataType::None:     return 0;
			case ShaderDataType::Bool:     return 1;
			case ShaderDataType::Int:      return 1;
			case ShaderDataType::Float:    return 1;
			case ShaderDataType::Vector2:  return 2;
			case ShaderDataType::Vector3:  return 3;
			case ShaderDataType::Vector4:  return 4;
			case ShaderDataType::IVector2: return 2;
			case ShaderDataType::IVector3: return 3;
			case ShaderDataType::IVector4: return 4;
			case ShaderDataType::UVector2: return 2;
			case ShaderDataType::UVector3: return 3;
			case ShaderDataType::UVector4: return 4;
			case ShaderDataType::DVector2: return 2;
			case ShaderDataType::DVector3: return 3;
			case ShaderDataType::DVector4: return 4;
			case ShaderDataType::Matrix2:  return 2 * 2;
			case ShaderDataType::Matrix3:  return 3 * 3;
			case ShaderDataType::Matrix4:  return 4 * 4;
			case ShaderDataType::DMatrix2: return 2 * 2;
			case ShaderDataType::DMatrix3: return 3 * 3;
			case ShaderDataType::DMatrix4: return 4 * 4;
			}
			VOX_CORE_ASSERT(false, "Unknown shader data type");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(std::initializer_list<BufferElement> elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		inline uint32 GetStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		std::vector<BufferElement> m_Elements;
		uint32 m_Stride = 0;

		void CalculateOffsetsAndStride()
		{
			uint32 offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;

		virtual void SetData(const void* data, uint32 size) = 0;

		static Ref<VertexBuffer> Create(uint32 size);
		static Ref<VertexBuffer> Create(float* vertices, uint32 size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32 GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32* indices, uint32 count);
	};
}

