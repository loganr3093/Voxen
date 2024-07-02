#pragma once

#include <string>
#include "Voxen/Types/Types.h"

#include "glm/glm.hpp"

namespace Voxen
{

	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void UploadUniformMat4(const std::string name, const glm::mat4& matrix);
	private:
		ID m_RendererID;
	};

}