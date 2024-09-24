#pragma once

#include "Voxen/Core/Core.h"

#include "Voxen/VoxRenderer/Voxel.h"

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include <mutex>
#include <thread>

namespace Voxen
{
    class VoxelShape;

    // Octree Node class
    class OctreeNode
    {
    public:
        bool isLeaf = false; // True if this node is a leaf
        uint8 materialIndex = 255; // Material index if this is a leaf
        OctreeNode* children[8]; // Pointers to 8 child nodes if this is not a leaf

        // Constructor for a branch node (non-leaf)
        OctreeNode() : isLeaf(false), materialIndex(255)
        {
            for (int i = 0; i < 8; ++i)
            {
                children[i] = nullptr;
            }
        }

        // Destructor to clean up child nodes
        ~OctreeNode()
        {
            for (int i = 0; i < 8; ++i)
            {
                if (children[i] != nullptr)
                {
                    delete children[i];
                }
            }
        }
    };

    // Octree class
    class SparseVoxelOctree
    {
    public:
        // Constructor: Initialize octree with given bounds and depth
        SparseVoxelOctree(int depth);

        // Destructor
        ~SparseVoxelOctree();

        // Insert a voxel with material index
        void InsertVoxel(const IVector3& position, uint8 materialIndex);

        // Get the material index at a given voxel position
        uint8 GetVoxel(const IVector3& position);

        // Convert octree to dense 3D array (full grid)
        std::vector<std::vector<std::vector<uint8>>> ConvertToDenseArray();

    private:
        // Helper to calculate which octant a position belongs to
        int GetOctant(const IVector3& position, const IVector3& center);

        // Recursive helper for voxel insertion
        void InsertVoxelRecursive(OctreeNode* node, const IVector3& position, const IVector3& center, int size, uint8 materialIndex, int depth);

        // Recursive helper to get voxel material
        uint8 GetVoxelRecursive(OctreeNode* node, const IVector3& position, const IVector3& center, int size, int depth);

        // Helper to convert octree to a dense array
        void ConvertToDenseArrayRecursive(OctreeNode* node, std::vector<std::vector<std::vector<uint8>>>& grid, const IVector3& position, int size);
    private:
        OctreeNode* m_Root;
        int m_MaxDepth;
        AABB m_Bounds;
    private:
        friend VoxelShape;
    };
}