#pragma once

namespace Voxen
{
	class Entity;
	struct SparseVoxelTree;
	struct GPUSparseVoxelTree;
	struct GPUSparseVoxelTreeNode;

	class VoxMemoryAllocator
	{
	public:
		static void Allocate(const Entity& entity);
		static void Deallocate(const Entity& entity);

		static size_t Count();

		static const bool IsDirty();
		static void Flush();

		static const std::vector<GPUSparseVoxelTree>		GetTreeData();
		static const std::vector<GPUSparseVoxelTreeNode>	GetNodeData();
		static const std::vector<uint32>					GetLeafData();
		static const std::vector<Vector4>					GetPaletteData();

		static void PrintStats();
		static void PrintMemory();

	private:
		static void GenerateData();

		static void AddTree(const SparseVoxelTree tree, uint32& nodeOffset, uint32& leafOffset);
	};
}