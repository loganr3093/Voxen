#pragma once

#include "Voxen/Renderer/Shader.h"
#include "Voxen/Types/Types.h"

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace Voxen
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32 count) override;
		virtual void SetVector3(const std::string& name, const glm::vec3& value) override;
		virtual void SetVector4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual const std::string& GetName() const override { return m_Name; }

		void UploadUniformInt(const std::string& name, const int value);
		void UploadUniformIntArray(const std::string& name, const int* values, uint32 count);
		void UploadUniformFloat(const std::string& name, const float value);
		void UploadUniformVector2(const std::string& name, const glm::vec2& values);
		void UploadUniformVector3(const std::string& name, const glm::vec3& values);
		void UploadUniformVector4(const std::string& name, const glm::vec4& values);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		std::string ReadFile(const std::string& filePath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		ID m_RendererID;
		std::string m_Name;
	};
}