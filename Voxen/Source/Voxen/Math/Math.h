#pragma once

#include <glm/glm.hpp>

namespace Voxen::Math
{

	bool DecomposeTransform(const Matrix4& transform, Vector3& outTranslation, Vector3& outRotation, Vector3& outScale);

}