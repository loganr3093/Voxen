#pragma once

#include "Voxen/Core/Core.h"

#include "Voxen/VoxRenderer/Voxel.h"

#include <vector>

#include "Voxen/Utilities/VoxParser.h"

namespace Voxen
{
    class VoxelShape;

    // Sparse Voxel 64-Tree Node
    struct [[gnu::packed]] SparseVoxelTreeNode
    {
        uint32 IsLeaf : 1;     // Indicates if this node is a leaf containing plain voxels.
        uint32 ChildPtr : 31;  // Absolute offset to array of existing child nodes/voxels.
        uint64 ChildMask;      // Indicates which children/voxels are present in array.
    };

	// Sparse Voxel 64-Tree
    class SparseVoxelTree
    {
    public:
        SparseVoxelTree(const VoxelMap& voxelMap);

        void GenerateTree(const VoxelMap& voxelMap);

        size_t GetTotalVoxels() const;

        uint8_t At(int32 x, int32 y, int32 z) const;

    private:
        SparseVoxelTreeNode generateTree(const VoxelMap& voxelMap, int32 scale, glm::ivec3 pos);

        uint64 packBits64(const uint8* data);

        void leftPack(uint8* data, uint64 mask);

        uint8_t at(const SparseVoxelTreeNode& node, int32_t scale, glm::ivec3 pos, int32_t x, int32_t y, int32_t z) const;

	private:
        SparseVoxelTreeNode root;
        std::vector<SparseVoxelTreeNode> nodePool;
        std::vector<uint8_t> leafData;
        
    private:
		friend VoxelShape;
    };
}