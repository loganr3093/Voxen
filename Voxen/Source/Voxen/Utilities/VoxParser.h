#pragma once
#include <vector>
#include <cstdint>
#include <filesystem>

struct ogt_vox_matl;
struct ogt_vox_rgba;

namespace Voxen
{
    struct VoxelMap
    {
        std::vector<uint8_t> voxels;
        uint32_t size_x;
        uint32_t size_y;
        uint32_t size_z;

        std::vector<ogt_vox_matl> material_map;

        std::vector<ogt_vox_rgba> palette;

        float getMetal(uint8_t index) const;
        float getRough(uint8_t index) const;
        float getSpec(uint8_t index) const;
        float getIOR(uint8_t index) const;

        uint8_t paletteBaseOffset;

        void PrintMap();
    };

    class VoxLoader
    {
    public:
        static Ref<VoxelMap> Load(std::filesystem::path filePath);
    };
}