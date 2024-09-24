#pragma once
#include "Voxen/VoxRenderer/VoxelShape.h"

namespace Voxen
{
	class VoxMemoryAllocator
	{
	public:
		static void Allocate(const Ref<VoxelShape>& shape);
		static void Deallocate(const Ref<VoxelShape>& shape);

		static const Ref<VoxelShape>& GetShape(size_t index);
		
		static size_t Count();

		static void GenerateBuffers();

		static std::vector<GPUVoxelShape> GetShapeBuffer();
		static std::vector<uint32> GetVoxelBuffer();

	};
}