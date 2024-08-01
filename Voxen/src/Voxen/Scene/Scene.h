#pragma once

#include <entt.hpp>

namespace Voxen
{
	class Scene
	{
	public:
		Scene();
		~Scene();
	private:
		entt::registry m_Registry;
	};
}