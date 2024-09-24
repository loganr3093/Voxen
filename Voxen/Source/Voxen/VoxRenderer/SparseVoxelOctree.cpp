#include "voxpch.h"
#include "Voxen/VoxRenderer/SparseVoxelOctree.h"

namespace Voxen
{
    SparseVoxelOctree::SparseVoxelOctree(int depth)
        : m_Bounds(IVector3(0), IVector3(std::pow(2, depth))), m_MaxDepth(depth)
    {
        m_Root = new OctreeNode();
    }

    SparseVoxelOctree::~SparseVoxelOctree()
    {
        Clear();
        delete m_Root;
    }

    void SparseVoxelOctree::InsertVoxel(const IVector3& position, uint8 materialIndex)
    {
        if (materialIndex == Voxel::EMPTY_VOXEL)
        {
            VOX_CORE_WARN("Attempting to insert the empty voxel (material index {0}). Operation ignored", static_cast<int>(Voxel::EMPTY_VOXEL));
            return;
        }

        IVector3 center = m_Bounds.Center();
        int size = (m_Bounds.max.x - m_Bounds.min.x) / 2;
        InsertVoxelRecursive(m_Root, position, center, size, materialIndex, 0);
    }

    uint8 SparseVoxelOctree::GetVoxel(const IVector3& position)
    {
        if (!m_Bounds.Contains(position))
        {
            return Voxel::EMPTY_VOXEL;
        }

        IVector3 center = m_Bounds.Center();
        int size = (m_Bounds.max.x - m_Bounds.min.x) / 2;
        return GetVoxelRecursive(m_Root, position, center, size, 0);
    }

    std::vector<std::vector<std::vector<uint8>>> SparseVoxelOctree::ConvertToDenseArray()
    {
        std::vector<std::vector<std::vector<uint8>>> grid(m_Bounds.max.x, std::vector<std::vector<uint8_t>>(m_Bounds.max.y, std::vector<uint8>(m_Bounds.max.z, Voxel::EMPTY_VOXEL)));
        ConvertToDenseArrayRecursive(m_Root, grid, IVector3(0, 0, 0), m_Bounds.max.x);
        return grid;
    }

    void SparseVoxelOctree::Clear()
    {
        ClearRecursive(m_Root);
        delete m_Root;
        m_Root = new OctreeNode();
    }

    int SparseVoxelOctree::GetOctant(const IVector3& position, const IVector3& center)
    {
        return
            (position.x >= center.x ? 1 : 0) |
            (position.y >= center.y ? 2 : 0) |
            (position.z >= center.z ? 4 : 0);
    }

    void SparseVoxelOctree::InsertVoxelRecursive(OctreeNode* node, const IVector3& position, const IVector3& center, int size, uint8 materialIndex, int depth)
    {
        if (depth == m_MaxDepth)
        {
            node->isLeaf = true;
            node->materialIndex = materialIndex;
            return;
        }

        int octant = GetOctant(position, center);
        IVector3 newCenter = center;

        // Calculate the new center of the octant
        int halfSize = size / 2;
        if (octant & 1) newCenter.x += halfSize;
        else newCenter.x -= halfSize;
        if (octant & 2) newCenter.y += halfSize;
        else newCenter.y -= halfSize;
        if (octant & 4) newCenter.z += halfSize;
        else newCenter.z -= halfSize;

        // If no child exists for this octant, create one
        if (node->children[octant] == nullptr)
        {
            node->children[octant] = new OctreeNode();
        }

        // Recurse into the child
        InsertVoxelRecursive(node->children[octant], position, newCenter, halfSize, materialIndex, depth + 1);
    }

    uint8 SparseVoxelOctree::GetVoxelRecursive(OctreeNode* node, const IVector3& position, const IVector3& center, int size, int depth)
    {
        if (node == nullptr)
        {
            return Voxel::EMPTY_VOXEL;
        }

        if (node->isLeaf)
        {
            return node->materialIndex;
        }

        int octant = GetOctant(position, center);
        IVector3 newCenter = center;
        int halfSize = size / 2;

        if (octant & 1) newCenter.x += halfSize;
        else newCenter.x -= halfSize;
        if (octant & 2) newCenter.y += halfSize;
        else newCenter.y -= halfSize;
        if (octant & 4) newCenter.z += halfSize;
        else newCenter.z -= halfSize;

        return GetVoxelRecursive(node->children[octant], position, newCenter, halfSize, depth + 1);
    }

    void SparseVoxelOctree::ConvertToDenseArrayRecursive(OctreeNode* node, std::vector<std::vector<std::vector<uint8>>>& grid, const IVector3& position, int size)
    {
        if (node == nullptr)
        {
            return;
        }

        if (node->isLeaf)
        {
            for (int dx = 0; dx < size; ++dx)
            {
                for (int dy = 0; dy < size; ++dy)
                {
                    for (int dz = 0; dz < size; ++dz)
                    {
                        grid[position.x + dx][position.y + dy][position.z + dz] = node->materialIndex;
                    }
                }
            }
        }
        else
        {
            int halfSize = size / 2;
            for (int i = 0; i < 8; ++i)
            {
                int offsetX = (i & 1) ? halfSize : 0;
                int offsetY = (i & 2) ? halfSize : 0;
                int offsetZ = (i & 4) ? halfSize : 0;
                IVector3 newPos = IVector3(position.x + offsetX, position.y + offsetY, position.z + offsetZ);
                ConvertToDenseArrayRecursive(node->children[i], grid, newPos, halfSize);
            }
        }
    }

    void SparseVoxelOctree::ClearRecursive(OctreeNode* node)
    {
        if (!node) return;

        for (int i = 0; i < 8; ++i)
        {
            if (node->children[i])
            {
                ClearRecursive(node->children[i]);
                delete node->children[i];
                node->children[i] = nullptr;
            }
        }
    }
}
