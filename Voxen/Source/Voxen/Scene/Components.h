#pragma once

#include "Voxen/Scene/SceneCamera.h"
#include "Voxen/Scene/ScriptableEntity.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

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
        Vector3 Translation = { 0.0f, 0.0f, 0.0f };
        Vector3 Rotation = { 0.0f, 0.0f, 0.0f };
        Vector3 Scale = { 1.0f, 1.0f, 1.0f };


        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const Vector3& translation)
            : Translation(translation) {}

        Matrix4 GetTransform() const
        {
            Matrix4 translation = Matrix4(1.0f);
            translation = glm::translate(translation, Translation);

            Matrix4 rotation = glm::toMat4(Quaternion(Rotation));

            Matrix4 scale = Matrix4(1.0f);
            scale = glm::scale(scale, Scale);

            return translation * rotation * scale;
        }
    };

    struct SpriteRendererComponent
    {
        Vector4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;
        SpriteRendererComponent(const Vector4& color)
            : Color(color) {}

        operator Vector4& () { return Color; }
        operator const Vector4& () const { return Color; }
    };

    struct CameraComponent
    {
        SceneCamera Camera;
        bool Primary = true; // TODO: think about moving to Scene
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };

    struct NativeScriptComponent
    {
        ScriptableEntity* Instance = nullptr;

        ScriptableEntity* (*InstantiateScript)();
        void(*DestroyInstanceFunction)(NativeScriptComponent*);

        template<typename T>
        void Bind()
        {
            InstantiateScript = []() { return static_cast<ScriptableEntity*>( new T()); };
            DestroyInstanceFunction = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
        }
    };
}