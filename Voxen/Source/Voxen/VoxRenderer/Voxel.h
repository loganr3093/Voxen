#pragma once
#include <Voxen/Types/Types.h>

namespace Voxen
{
    namespace Voxel
    {
        const static uint8 EMPTY_VOXEL = 255;
    }

    struct AABB
    {
        Vector3 min;
        Vector3 max;

        // Constructors
        AABB() : min(0.0f), max(0.0f) {}
        AABB(const Vector3& min, const Vector3& max) : min(min), max(max) {}

        // Equality operator
        bool operator==(const AABB& other) const
        {
            return min == other.min && max == other.max;
        }

        int Length() { return min.x - max.x; }
        int Width() { return min.y - max.y; }
        int Height() { return min.z - max.z; }

        Vector3 Center()
        {
            return Vector3
            (
                min.x + (max.x - min.x) / 2,
                min.y + (max.y - min.y) / 2,
                min.z + (max.z - min.z) / 2
            );
        }

        bool Contains(Vector3 position)
        {
            return
            (
                position.x >= min.x &&
                position.y >= min.y &&
                position.z >= min.z &&
                position.x <= max.x &&
                position.y <= max.y &&
                position.z <= max.z
            );
        }
    };

    struct VoxelMaterial
    {
        uint8 red, green, blue;
        uint8 reflectivity, roughness, metallic, emissive;

        // Constructors
        VoxelMaterial() : red(0), green(0), blue(0), reflectivity(0), roughness(0), metallic(0), emissive(0) {}
        VoxelMaterial(uint8 r, uint8 g, uint8 b, uint8 reflect, uint8 rough, uint8 metal, uint8 emiss)
            : red(r), green(g), blue(b), reflectivity(reflect), roughness(rough), metallic(metal), emissive(emiss) {}

        // Equality operator
        bool operator==(const VoxelMaterial& other) const
        {
            return red == other.red && green == other.green && blue == other.blue &&
                reflectivity == other.reflectivity && roughness == other.roughness &&
                metallic == other.metallic && emissive == other.emissive;
        }
    };

    struct GPUVoxelMaterial
    {
        uint32 rgb;  // Packed RGB values
        uint32 rrme; // Packed reflectivity, roughness, metallic, emissive

        // Constructors
        GPUVoxelMaterial() : rgb(0), rrme(0) {}
        GPUVoxelMaterial(uint32 rgb_val, uint32 rrme_val) : rgb(rgb_val), rrme(rrme_val) {}

        // Equality operator
        bool operator==(const GPUVoxelMaterial& other) const
        {
            return rgb == other.rgb && rrme == other.rrme;
        }
    };
    struct GPUVoxelShape
    {
        Matrix4 transform;                  // Transformation matrix
        AABB aabb;                          // Bounding box
        GPUVoxelMaterial materials[256];    // Fixed-size array of materials
        uint32 materialMapOffset;              // Starting index of this shape's voxelMap in the global voxelMap buffer
        uint32 materialMapSize;                // Number of elements in the voxelMap for this shape

        // Constructors
        GPUVoxelShape() : transform(Matrix4(1.0f)), aabb(), materialMapOffset(0), materialMapSize(0) {}
        GPUVoxelShape(const Matrix4& trans, const AABB& bbox, const GPUVoxelMaterial mat[256], uint32 mapOffset, uint32 mapSize)
            : transform(trans), aabb(bbox), materialMapOffset(mapOffset), materialMapSize(mapSize)
        {
            for (int i = 0; i < 256; ++i) {
                materials[i] = mat[i];
            }
        }

        // Copy constructor
        GPUVoxelShape(const GPUVoxelShape& other)
            : transform(other.transform), aabb(other.aabb), materialMapOffset(other.materialMapOffset), materialMapSize(other.materialMapSize)
        {
            for (int i = 0; i < 256; ++i) {
                materials[i] = other.materials[i];
            }
        }

        // Move constructor
        GPUVoxelShape(GPUVoxelShape&& other) noexcept
            : transform(std::move(other.transform)), aabb(std::move(other.aabb)), materialMapOffset(other.materialMapOffset), materialMapSize(other.materialMapSize)
        {
            for (int i = 0; i < 256; ++i) {
                materials[i] = std::move(other.materials[i]);
            }
        }

        // Copy assignment operator
        GPUVoxelShape& operator=(const GPUVoxelShape& other)
        {
            if (this != &other) {
                transform = other.transform;
                aabb = other.aabb;
                materialMapOffset = other.materialMapOffset;
                materialMapSize = other.materialMapSize;
                for (int i = 0; i < 256; ++i) {
                    materials[i] = other.materials[i];
                }
            }
            return *this;
        }

        // Move assignment operator
        GPUVoxelShape& operator=(GPUVoxelShape&& other) noexcept
        {
            if (this != &other) {
                transform = std::move(other.transform);
                aabb = std::move(other.aabb);
                materialMapOffset = other.materialMapOffset;
                materialMapSize = other.materialMapSize;
                for (int i = 0; i < 256; ++i) {
                    materials[i] = std::move(other.materials[i]);
                }
            }
            return *this;
        }

        // Equality operator
        bool operator==(const GPUVoxelShape& other) const
        {
            if (transform != other.transform || aabb != other.aabb || materialMapOffset != other.materialMapOffset || materialMapSize != other.materialMapSize)
                return false;

            for (int i = 0; i < 256; ++i) {
                if (materials[i] != other.materials[i]) {
                    return false;
                }
            }
            return true;
        }
    };
}