#include "voxpch.h"
#include "Platform/OpenGL/OpenGLComputeShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>

namespace Voxen
{
	OpenGLComputeShader::OpenGLComputeShader(const std::string& filePath)
	{
		VOX_PROFILE_FUNCTION();

		std::string source = ReadFile(filePath);
		VOX_TRACE("Compiling compute shader '{0}'", filePath);
		Compile(source);

		// Extract name from file path
		auto lastSlash = filePath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filePath.rfind('.');
		auto count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;

		m_Name = filePath.substr(lastSlash, count);
	}

	OpenGLComputeShader::OpenGLComputeShader(const std::string& name, const std::string& computeSrc)
		: m_Name(name)
	{
		VOX_PROFILE_FUNCTION();

		VOX_TRACE("Compiling compute shader from text");
		Compile(computeSrc);
	}

	OpenGLComputeShader::~OpenGLComputeShader()
	{
		VOX_PROFILE_FUNCTION();
		glDeleteProgram(m_RendererID);
	}

	void OpenGLComputeShader::Dispatch(uint32 xGroups, uint32 yGroups, uint32 zGroups) const
	{
		VOX_PROFILE_FUNCTION();
		glDispatchCompute(xGroups, yGroups, zGroups);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	std::string OpenGLComputeShader::ReadFile(const std::string& filePath)
	{
		VOX_PROFILE_FUNCTION();

		std::string result;
		std::ifstream in(filePath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			VOX_CORE_ERROR("Could not open file '{0}'", filePath);
		}

		return result;
	}

	void OpenGLComputeShader::Compile(const std::string& source)
	{
		VOX_PROFILE_FUNCTION();

		GLuint program = glCreateProgram();
		GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);

		const GLchar* sourceCStr = source.c_str();
		glShaderSource(computeShader, 1, &sourceCStr, 0);
		glCompileShader(computeShader);

		GLint isCompiled = 0;
		glGetShaderiv(computeShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(computeShader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(computeShader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(computeShader);

			VOX_CORE_ERROR("{0}", infoLog.data());
			VOX_CORE_ASSERT(false, "Compute shader compilation failure!");
			return;
		}

		glAttachShader(program, computeShader);
		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);
			glDeleteShader(computeShader);

			VOX_CORE_ERROR("{0}", infoLog.data());
			VOX_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		glDetachShader(program, computeShader);
		glDeleteShader(computeShader);

		m_RendererID = program;
	}

	void OpenGLComputeShader::Bind() const
	{
		VOX_PROFILE_FUNCTION();
		glUseProgram(m_RendererID);
	}

	void OpenGLComputeShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLComputeShader::SetInt(const std::string& name, int value)
	{
		VOX_PROFILE_FUNCTION();
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLComputeShader::SetIntArray(const std::string& name, int* values, uint32 count)
	{
		VOX_PROFILE_FUNCTION();
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLComputeShader::SetFloat(const std::string& name, float value)
	{
		VOX_PROFILE_FUNCTION();
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLComputeShader::SetVector2(const std::string& name, const Vector2& value)
	{
		VOX_PROFILE_FUNCTION();
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLComputeShader::SetVector3(const std::string& name, const Vector3& value)
	{
		VOX_PROFILE_FUNCTION();
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLComputeShader::SetVector4(const std::string& name, const Vector4& value)
	{
		VOX_PROFILE_FUNCTION();
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLComputeShader::SetMat4(const std::string& name, const Matrix4& value)
	{
		VOX_PROFILE_FUNCTION();
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
}
