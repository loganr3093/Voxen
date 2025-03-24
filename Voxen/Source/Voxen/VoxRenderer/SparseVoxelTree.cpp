#include "voxpch.h"
#include "Voxen/VoxRenderer/SparseVoxelTree.h"

namespace Voxen
{
    SparseVoxelTree::SparseVoxelTree()
        : root(), AABBMin(), AABBMax(), Transform()
    {
    }

    SparseVoxelTree::SparseVoxelTree(const Ref<VoxelMap> voxelMap)
    {
        // Clear existing data
        nodePool.clear();
        leafData.clear();

        // Start generating the tree from the root
        root = generateTree(voxelMap, 6, IVector3(0, 0, 0));
    }

    SparseVoxelTree::SparseVoxelTree(const std::filesystem::path modelPath)
    {
        // Make the voxel map from the path
        Ref<VoxelMap> voxelMap = VoxLoader::Load(modelPath);

        // Clear existing data
        nodePool.clear();
        leafData.clear();

        // TODO: Move this to appropriate places
        AABBMin = Vector3(0.0f, 0.0f, 0.0f);
        AABBMax = Vector3(voxelMap->size_x, voxelMap->size_y, voxelMap->size_z);
        Transform = glm::mat4(1.0f);

        // Start generating the tree from the root
        root = generateTree(voxelMap, 6, IVector3(0, 0, 0));
    }

    SparseVoxelTreeNode SparseVoxelTree::generateTree(const Ref<VoxelMap> voxelMap, int32 scale, IVector3 pos)
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

                if (x < voxelMap->size_x && y < voxelMap->size_y && z < voxelMap->size_z) {
                    int32_t index = x + y * voxelMap->size_x + z * voxelMap->size_x * voxelMap->size_y;
                    temp[i] = voxelMap->voxels[index];
                }
            }

            node.IsLeaf = 1;
            node.ChildMask = packBits64(temp);

            leftPack(temp, node.ChildMask);
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

    uint64_t SparseVoxelTree::packBits64(const uint8* data)
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

    void SparseVoxelTree::leftPack(uint8* data, uint64 mask)
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
}