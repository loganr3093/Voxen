#pragma once
#include "Voxen/VoxRenderer/SparseVoxelOctree.h"

#include "Voxen/Types/Types.h"

namespace Voxen
{
	class VoxelShape
	{
	public:
		VoxelShape();
		~VoxelShape();

		void SetMaterial(uint8 index, VoxelMaterial material);
		void InsertVoxel(IVector3 position, uint8 materialIndex);
		void InsertVoxel(uint32 x, uint32 y, uint32 z, uint8 materialIndex);

		std::vector<std::vector<std::vector<uint8>>> GetGrid() { return m_Octree.ConvertToDenseArray(); }

		AABB Bounds() { return m_Octree.m_Bounds; }

	private:
		Matrix4 m_Transform;
		SparseVoxelOctree m_Octree;
		std::unordered_map<uint8, VoxelMaterial> m_MaterialMap;
	};
}