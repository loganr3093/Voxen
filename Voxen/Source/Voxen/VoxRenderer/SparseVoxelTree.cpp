#include "voxpch.h"
#include "Voxen/VoxRenderer/SparseVoxelTree.h"

namespace Voxen
{

    SparseVoxelTree::SparseVoxelTree(const VoxelMap& voxelMap)
    {
        GenerateTree(voxelMap);
    }

    void SparseVoxelTree::GenerateTree(const VoxelMap& voxelMap)
    {
        // Clear existing data
        nodePool.clear();
        leafData.clear();

        // Start generating the tree from the root
        root = generateTree(voxelMap, 6, glm::ivec3(0, 0, 0));
    }

    // Function to count the total number of voxels in the tree
    size_t SparseVoxelTree::GetTotalVoxels() const
    {
        return leafData.size();
    }

    // Function to get the voxel data at a specific coordinate
    uint8_t SparseVoxelTree::At(int32_t x, int32_t y, int32_t z) const
    {
        return at(root, 6, glm::ivec3(0, 0, 0), x, y, z);
    }

    SparseVoxelTreeNode SparseVoxelTree::generateTree(const VoxelMap& voxelMap, int32_t scale, glm::ivec3 pos)
    {
        SparseVoxelTreeNode node = {};

        // Create leaf
        if (scale == 2)
        {
            assert((pos.x | pos.y | pos.z) % 4 == 0);

            // Repack voxels into 4x4x4 tile
            alignas(64) uint8_t temp[64] = { 0 };

            for (int32_t i = 0; i < 64; ++i)
            {
                int32_t x = pos.x + (i & 3);
                int32_t y = pos.y + ((i >> 2) & 3);
                int32_t z = pos.z + ((i >> 4) & 3);

                if (x < voxelMap.size_x && y < voxelMap.size_y && z < voxelMap.size_z) {
                    int32_t index = x + y * voxelMap.size_x + z * voxelMap.size_x * voxelMap.size_y;
                    temp[i] = voxelMap.voxels[index];
                }
            }

            node.IsLeaf = 1;
            node.ChildMask = packBits64(temp); // Generate bitmask of `temp[i] != 0`.

            leftPack(temp, node.ChildMask); // "Remove" entries where respective mask bit is zero.
            node.ChildPtr = leafData.size();
            leafData.insert(leafData.end(), temp, temp + std::popcount(node.ChildMask));

            return node;
        }

        // Descend
        scale -= 2;

        std::vector<SparseVoxelTreeNode> children;

        for (int32_t i = 0; i < 64; ++i)
        {
            glm::ivec3 childPos = glm::ivec3((i & 3), ((i >> 2) & 3), ((i >> 4) & 3));
            SparseVoxelTreeNode child = generateTree(voxelMap, scale, pos + (childPos << scale));

            if (child.ChildMask != 0)
            {
                node.ChildMask |= 1ull << i;
                children.push_back(child);
            }
        }

        node.ChildPtr = nodePool.size();
        nodePool.insert(nodePool.end(), children.begin(), children.end());

        return node;
    }

    uint64_t SparseVoxelTree::packBits64(const uint8_t* data)
    {
        uint64_t mask = 0;
        for (int i = 0; i < 64; ++i)
        {
            if (data[i] != 0)
            {
                mask |= 1ull << i;
            }
        }
        return mask;
    }

    void SparseVoxelTree::leftPack(uint8_t* data, uint64_t mask)
    {
        int writeIndex = 0;
        for (int i = 0; i < 64; ++i)
        {
            if (mask & (1ull << i))
            {
                data[writeIndex++] = data[i];
            }
        }
    }

    uint8_t SparseVoxelTree::at(const SparseVoxelTreeNode& node, int32_t scale, glm::ivec3 pos, int32_t x, int32_t y, int32_t z) const
    {
        if (node.IsLeaf)
        {
            // Calculate the index within the 4x4x4 block
            int32_t localX = x - pos.x;
            int32_t localY = y - pos.y;
            int32_t localZ = z - pos.z;
            int32_t index = localX + localY * 4 + localZ * 16;

            // Check if the voxel exists
            if (node.ChildMask & (1ull << index))
            {
                // Calculate the index in the leafData array
                int32_t dataIndex = node.ChildPtr + std::popcount(node.ChildMask & ((1ull << index) - 1));
                return leafData[dataIndex];
            }
            else
            {
                return 0;
            }
        }
        else
        {
            // Calculate the child index
            int32_t childIndex = ((x - pos.x) >> (scale - 2)) + ((y - pos.y) >> (scale - 2)) * 4 + ((z - pos.z) >> (scale - 2)) * 16;

            // Check if the child exists
            if (node.ChildMask & (1ull << childIndex))
            {
                // Calculate the index in the nodePool array
                int32_t childPtr = node.ChildPtr + std::popcount(node.ChildMask & ((1ull << childIndex) - 1));
                return at(nodePool[childPtr], scale - 2, pos + glm::ivec3((childIndex & 3) << (scale - 2), ((childIndex >> 2) & 3) << (scale - 2), ((childIndex >> 4) & 3) << (scale - 2)), x, y, z);
            }
            else
            {
                return 0;
            }
        }
    }

}