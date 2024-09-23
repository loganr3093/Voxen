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
		if (static_cast<uint32>(position.x) & (1 << shift)) index |= 1;
		if (static_cast<uint32>(position.y) & (1 << shift)) index |= 2;
		if (static_cast<uint32>(position.z) & (1 << shift)) index |= 4;

		return index;
	}

	std::vector<CPUVoxel> SparseVoxelOctree::GetUniqueVoxels() const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		std::unordered_set<CPUVoxel, CPUVoxelHash> uniqueVoxelsSet;
		std::vector<CPUVoxel> uniqueVoxels;

		std::function<void(OctreeNode*)> collectVoxels = [&](OctreeNode* node)
		{
			if (!node) return;

			if (node->isLeaf)
			{
				if (uniqueVoxelsSet.insert(node->voxel).second)
				{
					uniqueVoxels.push_back(node->voxel);
				}
			}
			else
			{
				for (auto* child : node->children)
				{
					collectVoxels(child);
				}
			}
		};

		collectVoxels(m_Root);
		return uniqueVoxels;
	}

	CPUVoxelShape SparseVoxelOctree::ToCPUVoxelShape() const
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		// Get all unique voxels
		std::vector<CPUVoxel> uniqueVoxels = GetUniqueVoxels();

		// Define the maximum depth of the octree (assuming 32 levels for 32-bit coordinates)
		const int maxDepth = 31; // You can adjust this as necessary for your octree depth

		// Calculate the dimensions of the voxel grid based on the octree depth
		// Each depth level doubles the resolution, so the grid size is 2^maxDepth.
		int gridSize = static_cast<int>(std::pow(2, maxDepth)); // 2^maxDepth

		// 3D voxel map initialized to 255 (representing empty space)
		std::vector<uint8> voxelMap(gridSize * gridSize * gridSize, 255);

		// Helper function to compute linear index from 3D coordinates
		auto computeIndex = [&](int x, int y, int z) -> size_t
		{
			return static_cast<size_t>(x) + gridSize * (static_cast<size_t>(y) + gridSize * static_cast<size_t>(z));
		};

		// Helper function to recursively traverse the octree and fill the voxel map
		std::function<void(OctreeNode*, int, int, int, int)> fillVoxelMap;
		fillVoxelMap = [&](OctreeNode* node, int x, int y, int z, int depth)
		{
			if (!node) return;

			if (node->isLeaf)
			{
				// Find index of this voxel in the unique voxel vector
				auto it = std::find(uniqueVoxels.begin(), uniqueVoxels.end(), node->voxel);
				uint8 voxelIndex = static_cast<uint8>(std::distance(uniqueVoxels.begin(), it));

				// Set the voxel map for the current position
				voxelMap[computeIndex(x, y, z)] = voxelIndex;
			}
			else
			{
				// If not a leaf, recursively fill children
				int half = 1 << (maxDepth - depth - 1); // Half the size at the current depth
				for (int i = 0; i < 8; ++i)
				{
					int childX = x + (i & 1) * half;
					int childY = y + ((i >> 1) & 1) * half;
					int childZ = z + ((i >> 2) & 1) * half;
					fillVoxelMap(node->children[i], childX, childY, childZ, depth + 1);
				}
			}
		};

		// Start the traversal from the root of the octree at position (0, 0, 0)
		fillVoxelMap(m_Root, 0, 0, 0, 0);

		// Use an identity matrix for the transformation and a dummy AABB (can be updated to actual bounds)
		Matrix4 identityTransform(1.0f);
		AABB boundingBox(Vector3(0.0f), Vector3(static_cast<float>(gridSize)));

		return CPUVoxelShape(identityTransform, boundingBox, uniqueVoxels, voxelMap);
	}
}
