#include "voxpch.h"
#include "Voxen/Scripting/ScriptGlue.h"

#include "Voxen/Core/Input.h"
#include "Voxen/Core/KeyCodes.h"

#include "Voxen/Scripting/ScriptEngine.h"

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

namespace Voxen
{
	static std::unordered_map <MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFunctions;

#define VOX_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Voxen.InternalCalls::" #Name, Name)

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		VOX_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		VOX_CORE_ASSERT(entity);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		VOX_CORE_ASSERT(s_EntityHasComponentFunctions.find(managedType) != s_EntityHasComponentFunctions.end());
		return s_EntityHasComponentFunctions.at(managedType)(entity);
	}


	static void TransformComponent_GetTranslation(UUID entityID, Vector3* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		*outTranslation = entity.GetComponent<TransformComponent>().Translation;
	}

	static void TransformComponent_SetTranslation(UUID entityID, Vector3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		Entity entity = scene->GetEntityByUUID(entityID);
		entity.GetComponent<TransformComponent>().Translation = *translation;
	}

	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	template<typename ... Component>
	static void RegisterComponent()
	{
		([]()
		{
			std::string_view typeName = typeid(Component).name();
			size_t pos = typeName.find_last_of(':');
			std::string_view structName = typeName.substr(pos + 1);
			std::string managedTypename = fmt::format("Voxen.{}", structName);

			MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
			if (!managedType)
			{
				VOX_CORE_ERROR("Could not find component type {0}", managedTypename);
				return;
			}
			s_EntityHasComponentFunctions[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
		}(), ...);
	}

	template<typename ... Component>
	static void RegisterComponent(ComponentGroup<Component ...>)
	{
		RegisterComponent<Component ...>();
	}

	void ScriptGlue::RegisterComponents()
	{
		RegisterComponent(AllComponents{});
	}

	void ScriptGlue::RegisterFunctions()
	{
		VOX_ADD_INTERNAL_CALL(Entity_HasComponent);

		VOX_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		VOX_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

		VOX_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}
}