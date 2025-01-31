#pragma once
#include <vector>
#include <cstdint>
#include <filesystem>

struct VoxelMap
{
    std::vector<uint8_t> voxels;
    uint32_t size_x;
    uint32_t size_y;
    uint32_t size_z;
};

class VoxLoader
{
public:
    static VoxelMap Load(std::filesystem::path filePath);
};