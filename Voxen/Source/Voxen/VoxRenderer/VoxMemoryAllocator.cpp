#include "voxpch.h"
#include "Voxen/VoxRenderer/VoxMemoryAllocator.h"

namespace Voxen
{
	namespace Utils
	{
		GPUVoxelMaterial PackMaterial(const VoxelMaterial& material)
		{
			uint32 rgb = 0;
			rgb |= (static_cast<uint32>(material.red)   << 16);
			rgb |= (static_cast<uint32>(material.green) << 8);
			rgb |= (static_cast<uint32>(material.blue)  << 0);

			uint32 rrme = 0;
			rrme |= (static_cast<uint32>(material.reflectivity) << 24);
			rrme |= (static_cast<uint32>(material.roughness)    << 16);
			rrme |= (static_cast<uint32>(material.metallic)		<< 8);
			rrme |= (static_cast<uint32>(material.emissive)		<< 0);

			return GPUVoxelMaterial(rgb, rrme);
		}

		std::vector<uint32> PackGrid(const std::vector<uint8>& grid)
		{
			std::vector<uint32> packedGrid;

			// Calculate the size needed for the packed grid
			size_t packedSize = (grid.size() + 3) / 4; // Rounds up if not a multiple of 4
			packedGrid.reserve(packedSize);

			// Iterate through the input grid in steps of 4
			for (size_t i = 0; i < grid.size(); i += 4)
			{
				uint32_t packedValue = 0;

				// Pack up to four uint8 values into one uint32
				packedValue |= (i + 0 < grid.size() ? static_cast<uint32>(grid[i + 0]) : 0)	<< 24; // 1st byte (MSB)
				packedValue |= (i + 1 < grid.size() ? static_cast<uint32>(grid[i + 1]) : 0) << 16; // 2nd byte
				packedValue |= (i + 2 < grid.size() ? static_cast<uint32>(grid[i + 2]) : 0) << 8;  // 3rd byte
				packedValue |= (i + 3 < grid.size() ? static_cast<uint32>(grid[i + 3]) : 0) << 0;  // 4th byte (LSB)

				// Add the packed value to the output vector
				packedGrid.push_back(packedValue);
			}

			return packedGrid;
		}

		std::vector<uint8> FlattenGrid(const std::vector<std::vector<std::vector<uint8>>>& grid)
		{
			std::vector<uint8> flatGrid;

			// Reserve space for the flattened grid to improve performance
			size_t totalSize = grid.size() * grid[0].size() * grid[0][0].size();
			flatGrid.reserve(totalSize);

			// Iterate through x, y, and z in the correct order
			size_t z_size = grid.size();
			size_t y_size = grid[0].size();
			size_t x_size = grid[0][0].size();

			// Iterate in x, y, z order
			for (size_t z = 0; z < z_size; ++z)
			{
				for (size_t y = 0; y < y_size; ++y)
				{
					for (size_t x = 0; x < x_size; ++x)
					{
						flatGrid.push_back(grid[x][y][z]);
					}
				}
			}

			return flatGrid;
		}
	}

	struct MemoryAllocatorData
	{
		std::vector<Ref<VoxelShape>> shapes;

		std::vector<GPUVoxelShape> shapeBuffer;
		std::vector<uint32> voxelBuffer;
	};

	static MemoryAllocatorData s_Data;

	void VoxMemoryAllocator::Allocate(const Ref<VoxelShape>& shape)
	{
		s_Data.shapes.push_back(shape);
	}

	void VoxMemoryAllocator::Deallocate(const Ref<VoxelShape>& shape)
	{
		// Find the shape in the vector
		auto it = std::find(s_Data.shapes.begin(), s_Data.shapes.end(), shape);
		if (it != s_Data.shapes.end())
		{
			// Swap with the last element and pop for fast removal
			std::swap(*it, s_Data.shapes.back());
			s_Data.shapes.pop_back();
		}
	}

	const Ref<VoxelShape>& VoxMemoryAllocator::GetShape(size_t index)
	{
		// Return the VoxelShape pointer at the specified index
		if (index >= s_Data.shapes.size())
		{
			VOX_CORE_WARN("Index out of range: {0}", index);
			return nullptr;
		}
		return s_Data.shapes[index];
	}

	size_t VoxMemoryAllocator::Count()
	{
		return s_Data.shapes.size();
	}

	void VoxMemoryAllocator::GenerateBuffers()
	{
		// Clear existing data inside buffers
		s_Data.shapeBuffer.clear();
		s_Data.voxelBuffer.clear();

		// Create array of GPU voxel shapes
		std::vector<GPUVoxelShape> GPUShapes;

		// Loop through each allocated shape
		for (auto shape : s_Data.shapes)
		{
			// Assign basic data
			GPUVoxelShape GPUShape;
			GPUShape.transform = shape->GetTransform();
			GPUShape.aabb = shape->Bounds();

			// Loop through each material in the material map and pack it to the GPU shape's map
			for (uint8 i = 0; i < 255; ++i)
			{
				GPUShape.materials[i] = Utils::PackMaterial(shape->GetMaterial(i));
			}

			// Pack the shape's grid and find offset
			std::vector<uint8> grid = Utils::FlattenGrid(shape->GetGrid());
			std::vector<uint32> packedGrid = Utils::PackGrid(grid);
			uint32 offset = static_cast<uint32>(s_Data.voxelBuffer.size());

			// Append the packed grid to the voxel buffer
			s_Data.voxelBuffer.insert(s_Data.voxelBuffer.end(), packedGrid.begin(), packedGrid.end());

			// Store the offset and size
			GPUShape.materialMapOffset = offset;
			GPUShape.materialMapSize = packedGrid.size();
			
			s_Data.shapeBuffer.push_back(GPUShape);
		}
	}

	std::vector<GPUVoxelShape> VoxMemoryAllocator::GetShapeBuffer()
	{
		return s_Data.shapeBuffer;
	}

	std::vector<uint32> VoxMemoryAllocator::GetVoxelBuffer()
	{
		return s_Data.voxelBuffer;
	}

}
