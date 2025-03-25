#pragma once

#include "Voxen/Core/UUID.h"
#include "Voxen/VoxRenderer/SparseVoxelTree.h"
#include "Voxen/Scene/Entity.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Voxen
{
    class Entity;
    struct SparseVoxelTree;
    struct GPUSparseVoxelTree;
    struct GPUSparseVoxelTreeNode;

    class VoxMemoryAllocator
    {
    public:
        static void Allocate(Entity& entity);
        static void Deallocate(Entity& entity);
        static size_t Count();
        static void Clear();

        // Transform tracking API
        static bool HasTransformChanged(Entity& entity, const glm::mat4& currentTransform);
        static void MarkDirty(Entity& entity);
        static void UpdateStoredTransform(Entity& entity, const glm::mat4& transform);

        // Dirty state checks
        static bool IsStructureDirty();
        static bool IsDataDirty();
        static void Flush();
        static void Refresh();

        // Data access
        static const std::vector<GPUSparseVoxelTree> GetTreeData();
        static const std::vector<GPUSparseVoxelTreeNode> GetNodeData();
        static const std::vector<uint32> GetLeafData();
        static const std::vector<Vector4> GetPaletteData();

        static void PrintStats();
        static void PrintMemory();

        static uint8 GetCurrentPaletteSize();

    private:
        static void GenerateData();
        static void AddTree(Entity& entity, uint32& nodeOffset, uint32& leafOffset);

        struct MemoryAllocatorData
        {
            std::vector<Entity> entities;
            bool isStructureDirty = false;
            bool isDataDirty = false;

            // Transform tracking
            std::unordered_map<UUID, glm::mat4> lastTransforms;
            std::unordered_set<UUID> dirtyEntities;

            // GPU data
            std::vector<GPUSparseVoxelTree> treeData;
            std::vector<GPUSparseVoxelTreeNode> nodeData;
            std::vector<uint32> leafData;
            std::vector<Vector4> paletteData;

            uint32 nodeOffset = 0;
            uint32 leafOffset = 0;
        };

        static MemoryAllocatorData s_Data;
    };
}