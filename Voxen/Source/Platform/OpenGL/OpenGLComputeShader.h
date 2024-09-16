#pragma once

#include "Voxen/Renderer/ComputeShader.h"
#include "Voxen/Types/Types.h"

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace Voxen
{
	class OpenGLComputeShader : public ComputeShader
	{
	public:
		OpenGLComputeShader(const std::filesystem::path& filepath);
		OpenGLComputeShader(const std::string& filepath);
		OpenGLComputeShader(const std::string& name, const std::string& computeSrc);
		virtual ~OpenGLComputeShader();

		virtual void Dispatch(uint32 xGroups, uint32 yGroups, uint32 zGroups) const override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32 count) override;
		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetVector2(const std::string& name, const Vector2& value) override;
		virtual void SetVector3(const std::string& name, const Vector3& value) override;
		virtual void SetVector4(const std::string& name, const Vector4& value) override;
		virtual void SetMat4(const std::string& name, const Matrix4& value) override;

		virtual const std::string& GetName() const override { return m_Name; }
	private:
		std::string ReadFile(const std::filesystem::path& filepath);
		void Compile(const std::string& source);

		ID m_RendererID;
		std::string m_Name;
	};
}