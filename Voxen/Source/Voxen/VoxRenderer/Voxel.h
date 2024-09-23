#pragma once
#include <Voxen/Types/Types.h>

namespace Voxen
{
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
    };

    struct CPUVoxel
    {
        uint8 red, green, blue;
        uint8 reflectivity, roughness, metallic, emissive;

        // Constructors
        CPUVoxel() : red(0), green(0), blue(0), reflectivity(0), roughness(0), metallic(0), emissive(0) {}
        CPUVoxel(uint8 r, uint8 g, uint8 b, uint8 reflect, uint8 rough, uint8 metal, uint8 emiss)
            : red(r), green(g), blue(b), reflectivity(reflect), roughness(rough), metallic(metal), emissive(emiss) {}

        // Equality operator
        bool operator==(const CPUVoxel& other) const
        {
            return red == other.red && green == other.green && blue == other.blue &&
                reflectivity == other.reflectivity && roughness == other.roughness &&
                metallic == other.metallic && emissive == other.emissive;
        }
    };

    struct CPUVoxelShape
    {
        Matrix4 transform;
        AABB aabb;
        std::vector<CPUVoxel> voxels;
        std::vector<uint8> voxelMap;

        // Default constructor
        CPUVoxelShape() : transform(Matrix4(1.0f)), aabb(), voxels(), voxelMap() {}

        // Constructor with parameters
        CPUVoxelShape(const Matrix4& trans, const AABB& bbox, const std::vector<CPUVoxel>& vox, const std::vector<uint8>& voxel_map)
            : transform(trans), aabb(bbox), voxels(vox), voxelMap(voxel_map) {}

        // Copy constructor
        CPUVoxelShape(const CPUVoxelShape& other)
            : transform(other.transform), aabb(other.aabb), voxels(other.voxels), voxelMap(other.voxelMap) {}

        // Move constructor
        CPUVoxelShape(CPUVoxelShape&& other) noexcept
            : transform(std::move(other.transform)), aabb(std::move(other.aabb)),
            voxels(std::move(other.voxels)), voxelMap(std::move(other.voxelMap)) {}

        // Copy assignment operator
        CPUVoxelShape& operator=(const CPUVoxelShape& other)
        {
            if (this != &other)
            {
                transform = other.transform;
                aabb = other.aabb;
                voxels = other.voxels;
                voxelMap = other.voxelMap;
            }
            return *this;
        }

        // Move assignment operator
        CPUVoxelShape& operator=(CPUVoxelShape&& other) noexcept
        {
            if (this != &other) {
                transform = std::move(other.transform);
                aabb = std::move(other.aabb);
                voxels = std::move(other.voxels);
                voxelMap = std::move(other.voxelMap);
            }
            return *this;
        }

        // Equality operator
        bool operator==(const CPUVoxelShape& other) const
        {
            return transform == other.transform && aabb == other.aabb && voxels == other.voxels && voxelMap == other.voxelMap;
        }
    };

    struct GPUVoxel
    {
        uint32 rgb;  // Packed RGB values
        uint32 rrme; // Packed reflectivity, roughness, metallic, emissive

        // Constructors
        GPUVoxel() : rgb(0), rrme(0) {}
        GPUVoxel(uint32 rgb_val, uint32 rrme_val) : rgb(rgb_val), rrme(rrme_val) {}

        // Equality operator
        bool operator==(const GPUVoxel& other) const
        {
            return rgb == other.rgb && rrme == other.rrme;
        }
    };

    struct GPUVoxelShape
    {
        Matrix4 transform;    // Transformation matrix
        AABB aabb;            // Bounding box
        GPUVoxel voxels[256]; // Fixed-size array of voxels
        uint32 voxelMapOffset;  // Starting index of this shape's voxelMap in the global voxelMap buffer
        uint32 voxelMapSize;    // Number of elements in the voxelMap for this shape

        // Constructors
        GPUVoxelShape() : transform(Matrix4(1.0f)), aabb(), voxelMapOffset(0), voxelMapSize(0) {}
        GPUVoxelShape(const Matrix4& trans, const AABB& bbox, const GPUVoxel vox[256], uint32 mapOffset, uint32 mapSize)
            : transform(trans), aabb(bbox), voxelMapOffset(mapOffset), voxelMapSize(mapSize)
        {
            for (int i = 0; i < 256; ++i) {
                voxels[i] = vox[i];
            }
        }

        // Copy constructor
        GPUVoxelShape(const GPUVoxelShape& other)
            : transform(other.transform), aabb(other.aabb), voxelMapOffset(other.voxelMapOffset), voxelMapSize(other.voxelMapSize)
        {
            for (int i = 0; i < 256; ++i) {
                voxels[i] = other.voxels[i];
            }
        }

        // Move constructor
        GPUVoxelShape(GPUVoxelShape&& other) noexcept
            : transform(std::move(other.transform)), aabb(std::move(other.aabb)), voxelMapOffset(other.voxelMapOffset), voxelMapSize(other.voxelMapSize)
        {
            for (int i = 0; i < 256; ++i) {
                voxels[i] = std::move(other.voxels[i]);
            }
        }

        // Copy assignment operator
        GPUVoxelShape& operator=(const GPUVoxelShape& other)
        {
            if (this != &other) {
                transform = other.transform;
                aabb = other.aabb;
                voxelMapOffset = other.voxelMapOffset;
                voxelMapSize = other.voxelMapSize;
                for (int i = 0; i < 256; ++i) {
                    voxels[i] = other.voxels[i];
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
                voxelMapOffset = other.voxelMapOffset;
                voxelMapSize = other.voxelMapSize;
                for (int i = 0; i < 256; ++i) {
                    voxels[i] = std::move(other.voxels[i]);
                }
            }
            return *this;
        }

        // Equality operator
        bool operator==(const GPUVoxelShape& other) const
        {
            if (transform != other.transform || aabb != other.aabb || voxelMapOffset != other.voxelMapOffset || voxelMapSize != other.voxelMapSize)
                return false;

            for (int i = 0; i < 256; ++i) {
                if (voxels[i] != other.voxels[i]) {
                    return false;
                }
            }
            return true;
        }
    };

    struct CPUVoxelHash
    {
        std::size_t operator()(const CPUVoxel& voxel) const
        {
            return (std::size_t(voxel.red) << 56) | (std::size_t(voxel.green) << 48) |
                (std::size_t(voxel.blue) << 40) | (std::size_t(voxel.reflectivity) << 32) |
                (std::size_t(voxel.roughness) << 24) | (std::size_t(voxel.metallic) << 16) |
                std::size_t(voxel.emissive);
        }
    };

    class VoxelDataPacker
    {
    public:
        static GPUVoxel CPUVoxelToGPUVoxel(const CPUVoxel& voxel)
        {
            return
            {
                PackRGB(voxel.red, voxel.blue, voxel.green),
                PackRRME(voxel.reflectivity, voxel.roughness, voxel.metallic, voxel.emissive)
            };
        }

        static CPUVoxel GPUVoxelToCPUVoxel(const GPUVoxel& voxel)
        {
            Vector3 rgb = UnpackRGB(voxel.rgb);
            Vector4 rrme = UnpackRRME(voxel.rrme);
            return
            {
                static_cast<uint8>(rgb.x),
                static_cast<uint8>(rgb.y),
                static_cast<uint8>(rgb.z),
                static_cast<uint8>(rrme.x),
                static_cast<uint8>(rrme.y),
                static_cast<uint8>(rrme.z),
                static_cast<uint8>(rrme.w)
            };
        }

        static GPUVoxelShape CPUVoxelShapeToGPUVoxelShape(const CPUVoxelShape& cpu_shape, uint32 voxelMapOffset, uint32 voxelMapSize)
        {
            GPUVoxelShape gpu_shape;
            gpu_shape.transform = cpu_shape.transform;
            gpu_shape.aabb = cpu_shape.aabb;
            gpu_shape.voxelMapOffset = voxelMapOffset;
            gpu_shape.voxelMapSize = voxelMapSize;

            // Convert CPU voxels to GPU voxels, ensuring we only copy as many as the vector contains
            size_t voxel_count = std::min(cpu_shape.voxels.size(), size_t(255));
            for (size_t i = 0; i < voxel_count; ++i)
            {
                gpu_shape.voxels[i] = CPUVoxelToGPUVoxel(cpu_shape.voxels[i]);
            }

            // If fewer than 256 voxels are provided, fill the rest with default GPUVoxel
            for (size_t i = voxel_count; i < 256; ++i)
            {
                gpu_shape.voxels[i] = GPUVoxel(); // Default GPUVoxel
            }

            return gpu_shape;
        }

        static Vector3 UnpackRGB(uint32 rgb)
        {
            return
            {
                static_cast<float>((rgb >> 16) & 0xFF) / 255.0f,
                static_cast<float>((rgb >> 8) & 0xFF) / 255.0f,
                static_cast<float>(rgb & 0xFF) / 255.0f
            };
        }

        static uint32 PackRGB(uint8 r, uint8 g, uint8 b)
        {
            return  (static_cast<uint32>(r) << 16) |
                    (static_cast<uint32>(g) << 8)  |
                     static_cast<uint32>(b);
        }

        static Vector4 UnpackRRME(uint32 rrme)
        {
            return
            {
                static_cast<float>(static_cast<uint32>((rrme >> 24) & 0xFF)) / 255.0f,
                static_cast<float>(static_cast<uint32>((rrme >> 16) & 0xFF)) / 255.0f,
                static_cast<float>(static_cast<uint32>((rrme >> 8) & 0xFF)) / 255.0f,
                static_cast<float>(static_cast<uint32>(rrme & 0xFF)) / 255.0f
            };
        }

        static uint32 PackRRME(uint8 reflectivity, uint8 roughness, uint8 metallic, uint8 emissive)
        {
            return (static_cast<uint32>(reflectivity) << 24) |
                   (static_cast<uint32>(roughness)    << 16) |
                   (static_cast<uint32>(metallic)     << 8)  |
                    static_cast<uint32>(emissive);
        }

        static std::vector<uint8> UnpackVoxelIndices(uint32 packed)
        {
            return
            {
                static_cast<uint8>((packed >> 24) & 0xFF),
                static_cast<uint8>((packed >> 16) & 0xFF),
                static_cast<uint8>((packed >> 8) & 0xFF),
                static_cast<uint8>(packed & 0xFF)
            };
        }

        static uint32 PackVoxelIndices(uint8 i0, uint8 i1, uint8 i2, uint8 i3)
        {
            return (static_cast<uint32>(i0) << 24) |
                   (static_cast<uint32>(i1) << 16) |
                   (static_cast<uint32>(i2) << 8)  |
                    static_cast<uint32>(i3);
        }
    };
}