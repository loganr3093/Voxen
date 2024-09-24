#pragma once
#include "Voxen/VoxRenderer/SparseVoxelOctree.h"

#include "Voxen/VoxRenderer/Voxel.h"

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

		const VoxelMaterial GetMaterial(uint8 index) { return m_MaterialMap[index]; }

		const AABB Bounds() const { return m_Octree.m_Bounds; }

		const Matrix4 GetTransform() const { return m_Transform; }

	private:
		Matrix4 m_Transform;
		SparseVoxelOctree m_Octree;
		std::unordered_map<uint8, VoxelMaterial> m_MaterialMap;
	};
}