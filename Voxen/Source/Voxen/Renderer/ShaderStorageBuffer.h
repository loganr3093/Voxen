#pragma once

#include "Voxen/Core/Core.h"

namespace Voxen
{
    class ShaderStorageBuffer
    {
    public:
        virtual ~ShaderStorageBuffer() = default;

        virtual void Bind(uint32_t binding) const = 0;
        virtual void Unbind() const = 0;

        static Ref<ShaderStorageBuffer> Create(const void* data, uint32_t size);
    };
}
