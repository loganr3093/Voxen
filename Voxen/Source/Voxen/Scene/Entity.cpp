#include "voxpch.h"
#include "Voxen/Scene/Entity.h"

namespace Voxen
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}
}
