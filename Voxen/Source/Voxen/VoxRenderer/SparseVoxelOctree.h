#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include <mutex>
#include <thread>

namespace Voxen
{
    struct Voxel
    {
        uint8_t red, green, blue;
        uint8_t reflectivity, roughness, metallic, emissive;

        Voxel() = default;
        Voxel(const Voxel&) = default;
        Voxel(uint8_t r, uint8_t g, uint8_t b, uint8_t refl, uint8_t rough, uint8_t metal, uint8_t emiss)
            : red(r), green(g), blue(b), reflectivity(refl), roughness(rough), metallic(metal), emissive(emiss) {}

        bool operator==(const Voxel& other) const
        {
            return red == other.red && green == other.green && blue == other.blue &&
                reflectivity == other.reflectivity && roughness == other.roughness &&
                metallic == other.metallic && emissive == other.emissive;
        }
    };

    struct VoxelHash
    {
        std::size_t operator()(const Voxel& voxel) const
        {
            return (std::size_t(voxel.red) << 56) | (std::size_t(voxel.green) << 48) |
                   (std::size_t(voxel.blue) << 40) | (std::size_t(voxel.reflectivity) << 32) |
                   (std::size_t(voxel.roughness) << 24) | (std::size_t(voxel.metallic) << 16) |
                    std::size_t(voxel.emissive);
        }
    };

    class OctreeNode
    {
    public:
        bool isLeaf;
        Voxel voxel;
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
        void Insert(int x, int y, int z, const Voxel& voxel);
        void Insert(const Vector3& position, const Voxel& voxel);

        // Bulk insertion to optimize multiple voxel adds
        void BulkInsert(const std::vector<std::pair<Vector3, Voxel>>& voxelData);

        // Finding voxels
        bool Find(int x, int y, int z) const;
        bool Find(const Vector3& position) const;

        // Removing voxels
        void Remove(int x, int y, int z);
        void Remove(const Vector3& position);

        // Traversal with a user-provided function
        void Traverse(OctreeNode* node, int depth, void (*func)(OctreeNode*, int)) const;

        // Unique voxel retrieval
        const std::vector<Voxel>& FindUniqueVoxels() const;
    private:
        OctreeNode* insert(OctreeNode* node, const Vector3& position, const Voxel& voxel, int depth);
        OctreeNode* find(OctreeNode* node, const Vector3& position, int depth) const;
        OctreeNode* remove(OctreeNode* node, const Vector3& position, int depth);

        void findUniqueVoxels(OctreeNode* node) const;

        int getChildIndex(const Vector3& position, int depth) const;
    private:
        // Thread safety for multi-threaded voxel operations
        mutable std::mutex m_Mutex;

        // Root node and unique voxels
        OctreeNode* m_Root;
        mutable std::vector<Voxel> m_UniqueVoxels;
    };
}

namespace std
{
    template <>
    struct hash<Voxen::Voxel>
    {
        std::size_t operator()(const Voxen::Voxel& voxel) const
        {
            return (std::size_t(voxel.red) << 56) | (std::size_t(voxel.green) << 48) |
                (std::size_t(voxel.blue) << 40) | (std::size_t(voxel.reflectivity) << 32) |
                (std::size_t(voxel.roughness) << 24) | (std::size_t(voxel.metallic) << 16) |
                std::size_t(voxel.emissive);
        }
    };
}