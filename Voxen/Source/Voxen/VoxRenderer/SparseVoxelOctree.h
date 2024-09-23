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
    class OctreeNode
    {
    public:
        bool isLeaf;
        CPUVoxel voxel;
        std::vector<OctreeNode*> children;

        OctreeNode() : isLeaf(false), voxel({ 0, 0, 0, 0, 0, 0, 0 }), children(8, nullptr) {}
        ~OctreeNode()
        {
            for (auto child : children)
            {
                delete child;
                child = nullptr;
            }
        }
    };

    class SparseVoxelOctree
    {
    public:
        SparseVoxelOctree() : m_Root(new OctreeNode()) {}
        ~SparseVoxelOctree() { delete m_Root; }

        // Single voxel insertion
        void Insert(int x, int y, int z, const CPUVoxel& voxel);
        void Insert(const Vector3& position, const CPUVoxel& voxel);

        // Bulk insertion to optimize multiple voxel adds
        void BulkInsert(const std::vector<std::pair<Vector3, CPUVoxel>>& voxelData);

        // Finding voxels
        bool Find(int x, int y, int z) const;
        bool Find(const Vector3& position) const;

        // Removing voxels
        void Remove(int x, int y, int z);
        void Remove(const Vector3& position);

        // Traversal with a user-provided function
        void Traverse(OctreeNode* node, int depth, void (*func)(OctreeNode*, int)) const;

        // Unique voxel retrieval
        const std::vector<CPUVoxel>& FindUniqueVoxels() const;
    private:
        OctreeNode* insert(OctreeNode* node, const Vector3& position, const CPUVoxel& voxel, int depth);
        OctreeNode* find(OctreeNode* node, const Vector3& position, int depth) const;
        OctreeNode* remove(OctreeNode* node, const Vector3& position, int depth);

        void findUniqueVoxels(OctreeNode* node) const;

        int getChildIndex(const Vector3& position, int depth) const;
    private:
        // Thread safety for multi-threaded voxel operations
        mutable std::mutex m_Mutex;

        // Root node and unique voxels
        OctreeNode* m_Root;
        mutable std::vector<CPUVoxel> m_UniqueVoxels;
    };
}