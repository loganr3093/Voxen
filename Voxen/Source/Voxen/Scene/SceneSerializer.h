#pragma once

#include "Voxen/Scene/Scene.h"

namespace Voxen
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const std::filesystem::path& filepath);

		bool Deserialize(const std::filesystem::path& filepath);
	private:
		Ref<Scene> m_Scene;
	};

}