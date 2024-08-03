#pragma once

#include <glm/glm.hpp>

namespace Voxen
{
	class Camera
	{
	public:
		Camera(const glm::mat4& projection)
			: m_Projection(projection) {}

		const glm::mat4& GetProjection() const { return m_Projection; }

		// TODO Create other camera functions

	private:
		glm::mat4 m_Projection;
	};
}