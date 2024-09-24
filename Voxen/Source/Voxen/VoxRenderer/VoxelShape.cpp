#include "voxpch.h"
#include "Voxen/VoxRenderer/VoxelShape.h"

namespace Voxen
{
	VoxelShape::VoxelShape()
		: m_Transform(Matrix4(1.0f)), m_Octree(4)
	{
		for (uint8 i = 0; i < 255; ++i)
		{
			m_MaterialMap[i] = VoxelMaterial();
		}
	}

	VoxelShape::~VoxelShape()
	{
		m_Transform = Matrix4(1.0f);
		// m_Octree.Clear();
		m_MaterialMap.clear();
	}

	void VoxelShape::SetMaterial(uint8 index, VoxelMaterial material)
	{
		if (index == Voxel::EMPTY_VOXEL)
		{
			VOX_CORE_WARN("Attempting to set the empty voxel (index {0}). Operation ignored", Voxel::EMPTY_VOXEL);
			return;
		}

		m_MaterialMap[index] = material;
	}

	void VoxelShape::InsertVoxel(IVector3 position, uint8 materialIndex)
	{
		m_Octree.InsertVoxel(position, materialIndex);
	}

	void VoxelShape::InsertVoxel(uint32 x, uint32 y, uint32 z, uint8 materialIndex)
	{
		m_Octree.InsertVoxel(IVector3(x, y, z), materialIndex);
	}
}
