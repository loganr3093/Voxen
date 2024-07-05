#pragma once

#include "Voxen/Renderer/Shader.h"
#include "Voxen/Types/Types.h"

namespace Voxen
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		void UploadUniformInt(const std::string name, const int value);
		void UploadUniformFloat(const std::string name, const float value);
		void UploadUniformVector2(const std::string name, const glm::vec2& values);
		void UploadUniformVector3(const std::string name, const glm::vec3& values);
		void UploadUniformVector4(const std::string name, const glm::vec4& values);

		void UploadUniformMat3(const std::string name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string name, const glm::mat4& matrix);
	private:
		ID m_RendererID;
	};
}