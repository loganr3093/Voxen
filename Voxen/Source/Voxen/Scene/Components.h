#pragma once

#include <glm/glm.hpp>

#include "SceneCamera.h"
#include "ScriptableEntity.h"

// TODO Change to Component namespace and remove postfix 'Component' from every component
namespace Voxen
{
    struct TagComponent
    {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag)
            : Tag(tag) {}

        operator std::string& () { return Tag; }
        operator const std::string& () const { return Tag; }
    };

    struct TransformComponent
    {
        glm::mat4 Transform = { 1.0f };

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::mat4& transform)
            : Transform(transform) {}

        operator glm::mat4& () { return Transform; }
        operator const glm::mat4& () const { return Transform; }
    };

    struct SpriteRendererComponent
    {
        glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;
        SpriteRendererComponent(const glm::vec4& color)
            : Color(color) {}

        operator glm::vec4& () { return Color; }
        operator const glm::vec4& () const { return Color; }
    };

    struct CameraComponent
    {
        SceneCamera Camera;
        bool Primary = true; // TODO move this to scene
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };

    struct NativeScriptComponent
    {
        ScriptableEntity* Instance = nullptr;

        void(*InstantiateFunction)(ScriptableEntity*&) = nullptr;
        void(*DestroyInstanceFunction)(ScriptableEntity*&) = nullptr;

        void(*OnCreateFunction)(ScriptableEntity*) = nullptr;
        void(*OnDestroyFunction)(ScriptableEntity*) = nullptr;
        void(*OnUpdateFunction)(ScriptableEntity*, Timestep) = nullptr;

        template<typename T>
        void Bind()
        {
            InstantiateFunction = [](ScriptableEntity*& instance) { instance = new T(); };
            DestroyInstanceFunction = [](ScriptableEntity*& instance) { delete static_cast<T*>(instance); instance = nullptr; };

            OnCreateFunction = [](ScriptableEntity* instance) { static_cast<T*>(instance)->OnCreate(); };
            OnDestroyFunction = [](ScriptableEntity* instance) { static_cast<T*>(instance)->OnDestroy(); };
            OnUpdateFunction = [](ScriptableEntity* instance, Timestep ts) { static_cast<T*>(instance)->OnUpdate(ts); };
        }
    };
}