#include "voxpch.h"
#include "Voxen/VoxRenderer/SparseVoxelOctree.h"

namespace Voxen
{
	void SparseVoxelOctree::Insert(int x, int y, int z, const CPUVoxel& voxel)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Root = insert(m_Root, { x, y, z }, voxel, 31);
	}

	void SparseVoxelOctree::Insert(const Vector3& position, const CPUVoxel& voxel)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Root = insert(m_Root, position, voxel, 31);
	}

	void SparseVoxelOctree::BulkInsert(const std::vector<std::pair<Vector3, CPUVoxel>>& voxelData)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		for (const auto& [position, voxel] : voxelData)
		{
			m_Root = insert(m_Root, position, voxel, 31);
		}
	}

	bool SparseVoxelOctree::Find(int x, int y, int z) const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		OctreeNode* node = find(m_Root, { x, y, z }, 31);
		return node && node->isLeaf;
	}

	bool SparseVoxelOctree::Find(const Vector3& position) const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		OctreeNode* node = find(m_Root, position, 31);
		return node && node->isLeaf;
	}

	void SparseVoxelOctree::Remove(int x, int y, int z)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Root = remove(m_Root, { x, y, z }, 31);
	}

	void SparseVoxelOctree::Remove(const Vector3& position)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Root = remove(m_Root, position, 31);
	}

	void SparseVoxelOctree::Traverse(OctreeNode* node, int depth, void(*func)(OctreeNode*, int)) const
	{
		if (!node) return;
		func(node, depth);

		for (auto child : node->children)
		{
			Traverse(child, depth - 1, func);
		}
	}

	const std::vector<CPUVoxel>& SparseVoxelOctree::FindUniqueVoxels() const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		findUniqueVoxels(m_Root);
		return m_UniqueVoxels;
	}

	OctreeNode* SparseVoxelOctree::insert(OctreeNode* node, const Vector3& position, const CPUVoxel& voxel, int depth)
	{
		// Bounds checking
		VOX_CORE_ASSERT(position.x >= 0 && position.y >= 0 && position.z >= 0, "Position out of range");

		if (depth == 0)
		{
			node->isLeaf = true;
			node->voxel = voxel;
			return node;
		}

		int index = getChildIndex(position, depth);
		if (!node->children[index])
		{
			node->children[index] = new OctreeNode();
		}

		node->children[index] = insert(node->children[index], position, voxel, depth - 1);
		return node;
	}

	OctreeNode* SparseVoxelOctree::find(OctreeNode* node, const Vector3& position, int depth) const
	{
		VOX_CORE_ASSERT(position.x >= 0 && position.y >= 0 && position.z >= 0, "Position out of range");

		if (!node || depth == 0) return node;

		int index = getChildIndex(position, depth);
		return find(node->children[index], position, depth - 1);
	}

	OctreeNode* SparseVoxelOctree::remove(OctreeNode* node, const Vector3& position, int depth)
	{
		VOX_CORE_ASSERT(position.x >= 0 && position.y >= 0 && position.z >= 0, "Position out of range");

		if (!node) return nullptr;

		if (depth == 0)
		{
			delete node;
			return nullptr;
		}

		int index = getChildIndex(position, depth);
		node->children[index] = remove(node->children[index], position, depth - 1);

		bool allChildrenNull = true;
		for (const auto& child : node->children)
		{
			if (child)
			{
				allChildrenNull = false;
				break;
			}
		}
		if (allChildrenNull)
		{
			delete node;
			return nullptr;
		}

		return node;
	}

	void SparseVoxelOctree::findUniqueVoxels(OctreeNode* node) const
	{
		if (!node) return;

		std::unordered_set<CPUVoxel, CPUVoxelHash> uniqueVoxelsSet;

		// Internal recursive function to traverse and collect unique voxels
		std::function<void(OctreeNode*)> collectVoxels = [&](OctreeNode* currentNode)
			{
				if (!currentNode) return;

				if (currentNode->isLeaf)
				{
					const CPUVoxel& voxel = currentNode->voxel;
					if (uniqueVoxelsSet.insert(voxel).second)
					{
						m_UniqueVoxels.push_back(voxel);
					}
				}
				else
				{
					for (auto* child : currentNode->children)
					{
						collectVoxels(child);
					}
				}
			};

		collectVoxels(node);
	}

	int SparseVoxelOctree::getChildIndex(const Vector3& position, int depth) const
	{
		int shift = 31 - depth;
		int index = 0;

		// Check if the bit at the current depth is set for each coordinate and adjust the index accordingly
		if (static_cast<uint32_t>(position.x) & (1 << shift)) index |= 1;
		if (static_cast<uint32_t>(position.y) & (1 << shift)) index |= 2;
		if (static_cast<uint32_t>(position.z) & (1 << shift)) index |= 4;

		return index;
	}
}
