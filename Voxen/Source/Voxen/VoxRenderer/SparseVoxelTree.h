#pragma once

#include "Voxen/Core/Core.h"

#include "Voxen/VoxRenderer/Voxel.h"

#include <vector>

#include "Voxen/Utilities/VoxParser.h"

namespace Voxen
{
    class VoxMemoryAllocator;

    // Sparse Voxel 64-Tree Node
    struct [[gnu::packed]] SparseVoxelTreeNode
    {
        uint32 IsLeaf : 1;     // Indicates if this node is a leaf containing plain voxels.
        uint32 ChildPtr : 31;  // Absolute offset to array of existing child nodes/voxels.
        uint64 ChildMask;      // Indicates which children/voxels are present in array.
    };

    class SparseVoxelTree
    {
    public:
        SparseVoxelTree();

        SparseVoxelTree(const Ref<VoxelMap> data);
        SparseVoxelTree(const std::filesystem::path modelPath);

    private:
        SparseVoxelTreeNode generateTree(const Ref<VoxelMap> data, int32 scale, IVector3 pos);

        uint64 packBits64(const uint8* data);

        void leftPack(uint8* data, uint64 mask);

    private:
        Ref<VoxelMap> voxelMap;

        // The sparse 64 tree structure members
        SparseVoxelTreeNode root;
        std::vector<SparseVoxelTreeNode> nodePool;
        std::vector<uint8_t> leafData;

        // TODO: Remove from SparseVoxelTree
        Vector3 AABBMin;
        Vector3 AABBMax;

    private:
        friend VoxMemoryAllocator;
    };

    // *************************
    // GPU Structures
    // *************************
    struct GPUSparseVoxelTreeNode
    {
        // PackedData[0]: Combines IsLeaf (1 bit) and ChildPtr (31 bits).
        // PackedData[1]: Lower 32 bits of ChildMask.
        // PackedData[2]: Upper 32 bits of ChildMask.
        // 12 bytes
        uint32_t PackedData[3];
    };

    struct GPUAABB
    {
        // Min bounds of the AABB
        // 16 bytes
        alignas(16) glm::vec4 Min;
        // Max bounds of the AABB
        // 16 bytes
        alignas(16) glm::vec4 Max;
    };

    struct GPUSparseVoxelTree
    {
        GPUSparseVoxelTreeNode Root;        // 12 bytes

        alignas(4) uint32_t NodePoolPtr;    // 4 bytes
        alignas(4) uint32_t LeafDataPtr;    // 4 bytes
        alignas(4) uint32_t PaletteDataPtr; // 4 bytes

        alignas(4) uint32_t _padding[2];    // 8 bytes

        alignas(16) GPUAABB Bounds;         // 32 bytes

        alignas(16) glm::mat4 Transform;    // 64 bytes
    };
}