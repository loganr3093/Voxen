#pragma once

#include <glm/glm.hpp>

#include "Voxen/Renderer/Camera.h"

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
        Camera Camera;
        bool Primary = true; // TODO move this to scene

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
        CameraComponent(const glm::mat4& projection)
            : Camera(projection) {}

    };
}