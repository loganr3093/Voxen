#include "voxpch.h"
#include "VoxParser.h"

#include "Voxen/VoxRenderer/VoxMemoryAllocator.h"

#define OGT_VOX_IMPLEMENTATION
#include "ogt/vox.h"

namespace Voxen
{
    Ref<VoxelMap> VoxLoader::Load(std::filesystem::path filePath)
    {
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file)
        {
            VOX_ERROR("Failed to open file in VoxLoader::Load.");
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size))
        {
            VOX_ERROR("Failed to read file in VoxLoader::Load.");
        }

        const ogt_vox_scene* scene = ogt_vox_read_scene(reinterpret_cast<uint8_t*>(buffer.data()), buffer.size());
        if (!scene)
        {
            VOX_ERROR("Failed to parse .vox file in VoxLoader::Load.");
        }

        const ogt_vox_model* model = scene->models[0];
        VoxelMap voxel_map;
        voxel_map.size_x = model->size_x;
        voxel_map.size_y = model->size_y;
        voxel_map.size_z = model->size_z;
        voxel_map.voxels.resize(voxel_map.size_x * voxel_map.size_y * voxel_map.size_z);
        std::copy(model->voxel_data, model->voxel_data + voxel_map.voxels.size(), voxel_map.voxels.begin());

        voxel_map.material_map.resize(256);
        for (uint32_t i = 0; i < 256; ++i)
        {
            voxel_map.material_map[i] = scene->materials.matl[i];
        }

        voxel_map.palette.resize(256);
        for (uint32_t i = 0; i < 256; ++i)
        {
            voxel_map.palette[i] = scene->palette.color[i];
        }

        ogt_vox_destroy_scene(scene);
        return CreateRef<VoxelMap>(voxel_map);
    }

    void VoxelMap::PrintMap()
    {
        size_t voxelMem = voxels.size() * sizeof(uint8);

        std::cout << "===== Voxel Map =====" << std::endl;
        std::cout << "Voxels: " << voxels.size() << " entries, " << voxelMem << " bytes" << std::endl;
        std::cout << "Size: " << "( " << size_x << ", " << size_y << ", " << size_z << " )" << std::endl;
    }

    float VoxelMap::getMetal(uint8_t index) const
    {
        if (index < material_map.size())
            return material_map[index].metal;
        return 0.0f;
    }

    float VoxelMap::getRough(uint8_t index) const
    {
        if (index < material_map.size())
            return material_map[index].rough;
        return 0.0f;
    }

    float VoxelMap::getSpec(uint8_t index) const
    {
        if (index < material_map.size())
            return material_map[index].spec;
        return 0.0f;
    }

    float VoxelMap::getIOR(uint8_t index) const
    {
        if (index < material_map.size())
            return material_map[index].ior;
        return 0.0f;
    }
}