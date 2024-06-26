#pragma once

#include <string>
#include "Voxen/Types/Types.h"

namespace Voxen
{

	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;
	private:
		ID m_RendererID;
	};

}