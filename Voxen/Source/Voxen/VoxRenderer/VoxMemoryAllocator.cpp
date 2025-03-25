#include "voxpch.h"
#include "Voxen/VoxRenderer/VoxMemoryAllocator.h"
#include "Voxen/VoxRenderer/SparseVoxelTree.h"

#include "ogt/vox.h"

#include "Voxen/Scene/Entity.h"
#include "Voxen/Scene/Components.h"
#include <bitset>

namespace Voxen
{
	namespace Utils
	{
		GPUVoxelMaterial PackMaterial(const VoxelMaterial& material)
		{
			uint32 rgb = 0;
			rgb |= (static_cast<uint32>(material.red) << 16);
			rgb |= (static_cast<uint32>(material.green) << 8);
			rgb |= (static_cast<uint32>(material.blue) << 0);

			uint32 rrme = 0;
			rrme |= (static_cast<uint32>(material.reflectivity) << 24);
			rrme |= (static_cast<uint32>(material.roughness) << 16);
			rrme |= (static_cast<uint32>(material.metallic) << 8);
			rrme |= (static_cast<uint32>(material.emissive) << 0);

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
				packedValue |= (i + 0 < grid.size() ? static_cast<uint32>(grid[i + 0]) : 0) << 24; // 1st byte (MSB)
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

	VoxMemoryAllocator::MemoryAllocatorData VoxMemoryAllocator::s_Data;

	void VoxMemoryAllocator::Allocate(Entity& entity)
	{
		s_Data.entities.push_back(entity);
		s_Data.isStructureDirty = true;

		// Store initial transform
		const auto& transform = entity.GetComponent<TransformComponent>().GetTransform();
		s_Data.lastTransforms[entity.GetUUID()] = transform;
	}

	void VoxMemoryAllocator::Deallocate(Entity& entity)
	{
		auto it = std::find(s_Data.entities.begin(), s_Data.entities.end(), entity);
		if (it != s_Data.entities.end()) {
			// Cleanup tracking data
			const UUID uuid = entity.GetUUID();
			s_Data.lastTransforms.erase(uuid);
			s_Data.dirtyEntities.erase(uuid);

			std::swap(*it, s_Data.entities.back());
			s_Data.entities.pop_back();
			s_Data.isStructureDirty = true;
		}
	}

	bool VoxMemoryAllocator::HasTransformChanged(Entity& entity, const glm::mat4& currentTransform)
	{
		const UUID uuid = entity.GetUUID();
		auto& transforms = s_Data.lastTransforms;

		if (transforms.find(uuid) == transforms.end())
			return false; // Should never happen for valid entities

		return transforms[uuid] != currentTransform;
	}

	void VoxMemoryAllocator::MarkDirty(Entity& entity)
	{
		s_Data.dirtyEntities.insert(entity.GetUUID());
		s_Data.isDataDirty = true;
	}

	void VoxMemoryAllocator::UpdateStoredTransform(Entity& entity, const glm::mat4& transform)
	{
		s_Data.lastTransforms[entity.GetUUID()] = transform;
	}

	// Dirty state management
	bool VoxMemoryAllocator::IsStructureDirty() { return s_Data.isStructureDirty; }
	bool VoxMemoryAllocator::IsDataDirty() { return s_Data.isDataDirty; }

	size_t VoxMemoryAllocator::Count()
	{
		return s_Data.entities.size();
	}

	void VoxMemoryAllocator::Clear()
	{
		s_Data.entities.clear();
		s_Data.isStructureDirty = true;
		s_Data.isDataDirty = true;

		// Transform tracking
		s_Data.lastTransforms.clear();
		s_Data.dirtyEntities.clear();

		// GPU data
		s_Data.treeData.clear();
		s_Data.nodeData.clear();
		s_Data.leafData.clear();
		s_Data.paletteData.clear();

		s_Data.nodeOffset = 0;
		s_Data.leafOffset = 0;
	}


	const std::vector<GPUSparseVoxelTree> VoxMemoryAllocator::GetTreeData()
	{
		Flush();

		return s_Data.treeData;
	}

	const std::vector<GPUSparseVoxelTreeNode> VoxMemoryAllocator::GetNodeData()
	{
		Flush();

		return s_Data.nodeData;
	}

	const std::vector<uint32> VoxMemoryAllocator::GetLeafData()
	{
		Flush();

		return s_Data.leafData;
	}

	const std::vector<Vector4> VoxMemoryAllocator::GetPaletteData()
	{
		Flush();

		return s_Data.paletteData;
	}

	void VoxMemoryAllocator::PrintStats()
	{
		Refresh();

		size_t treeMem = s_Data.treeData.size() * sizeof(GPUSparseVoxelTree);
		size_t nodeMem = s_Data.nodeData.size() * sizeof(GPUSparseVoxelTreeNode);
		size_t leafMem = s_Data.leafData.size() * sizeof(uint32);
		size_t paletteMem = s_Data.paletteData.size() * sizeof(Vector4);

		std::cout << "===== Voxel Tree Stats =====" << std::endl;
		std::cout << "Sparse Voxel Trees: " <<	s_Data.treeData.size() << " entries, " << treeMem << " bytes" << std::endl;
		std::cout << "Node Data: " <<			s_Data.nodeData.size() << " entries, " << nodeMem << " bytes" << std::endl;
		std::cout << "Leaf Data: " <<			s_Data.leafData.size() << " entries, " << leafMem << " bytes" << std::endl;
		std::cout << "Palette Data: " <<		s_Data.paletteData.size() << " entries, " << paletteMem << " bytes" << std::endl;
		std::cout << "Total Memory Usage: " 
			<< (treeMem + nodeMem + leafMem + paletteMem) / (1024.0) << " KB, "
			<< (treeMem + nodeMem + leafMem + paletteMem) / (1024.0 * 1024.0) << " MB"
			<< std::endl;
	}

	void VoxMemoryAllocator::PrintMemory()
	{
		Refresh();
		
		std::cout << "===== Voxel Tree Memory Allocation =====" << std::endl;

		std::cout << "\nGPU Sparse Voxel Trees (" << s_Data.treeData.size() << " entries):" << std::endl;
		for (size_t i = 0; i < s_Data.treeData.size(); ++i)
		{
			const auto& tree = s_Data.treeData[i];
			std::cout << "Tree " << i << ":\n";
			std::cout << "  NodePoolPtr: " << tree.NodePoolPtr << "\n";
			std::cout << "  LeafDataPtr: " << tree.LeafDataPtr << "\n";
			std::cout << "  PaletteDataPtr: " << tree.PaletteDataPtr << "\n";
			std::cout << "  AABBMin: (" << tree.Bounds.Min.x << ", " << tree.Bounds.Min.y << ", " << tree.Bounds.Min.z << ", " << tree.Bounds.Min.w << ")\n";
			std::cout << "  AABBMax: (" << tree.Bounds.Max.x << ", " << tree.Bounds.Max.y << ", " << tree.Bounds.Max.z << ", " << tree.Bounds.Max.w << ")\n";
		}

		std::cout << "\nGPU Node Pool (" << s_Data.nodeData.size() << " entries):" << std::endl;
		for (size_t i = 0; i < s_Data.nodeData.size(); ++i)
		{
			const auto& node = s_Data.nodeData[i];
			std::cout << "Node " << i << ": ";
			std::cout << "PackedData[0]: " << std::bitset<32>(node.PackedData[0]) << " ";
			std::cout << "PackedData[1]: " << std::bitset<32>(node.PackedData[1]) << " ";
			std::cout << "PackedData[2]: " << std::bitset<32>(node.PackedData[2]) << std::endl;
		}

		std::cout << "\nGPU Leaf Data (" << s_Data.leafData.size() << " bytes):" << std::endl;
		for (size_t i = 0; i < s_Data.leafData.size(); ++i)
		{
			if (i % 16 == 0) std::cout << "\n" << i << ": ";
			std::cout << static_cast<int>(s_Data.leafData[i]) << " ";
		}

		std::cout << "\n===== Palette Data =====" << std::endl;
		for (size_t i = 0; i < s_Data.paletteData.size(); ++i)
		{
			const auto& color = s_Data.paletteData[i];
			std::cout << "Index " << i << ": "
				<< "R=" << color.r << " "
				<< "G=" << color.g << " "
				<< "B=" << color.b
				<< " (Model: " << (i / 256)
				<< ", Local Index: " << (i % 256) << ")\n";
			if ((i + 1) % 256 == 0) std::cout << "----- End of Model Palette -----\n";
		}
		std::cout << "\n======================================\n";
	}

	uint8 VoxMemoryAllocator::GetCurrentPaletteSize()
	{
		return s_Data.paletteData.size();
	}

	void VoxMemoryAllocator::Flush()
	{
		if (s_Data.isStructureDirty)
		{
			GenerateData();
			s_Data.isStructureDirty = false;
			s_Data.isDataDirty = false;
		}
		else if (s_Data.isDataDirty)
		{
			// Partial update of transform matrices
			for (const UUID& uuid : s_Data.dirtyEntities)
			{
				auto it = std::find_if(s_Data.entities.begin(), s_Data.entities.end(),
					[&](Entity& e) { return e.GetUUID() == uuid; });

				if (it != s_Data.entities.end())
				{
					const size_t index = std::distance(s_Data.entities.begin(), it);
					const auto& transform = it->GetComponent<TransformComponent>().GetTransform();

					s_Data.treeData[index].Transform = transform;
					UpdateStoredTransform(*it, transform);
				}
			}
			s_Data.dirtyEntities.clear();
			s_Data.isDataDirty = false;
		}
	}

	void VoxMemoryAllocator::Refresh()
	{
		if (s_Data.isStructureDirty)
		{
			GenerateData();
		}
		else if (s_Data.isDataDirty)
		{
			// Partial update of transform matrices
			for (const UUID& uuid : s_Data.dirtyEntities)
			{
				auto it = std::find_if(s_Data.entities.begin(), s_Data.entities.end(),
					[&](Entity& e) { return e.GetUUID() == uuid; });

				if (it != s_Data.entities.end())
				{
					const size_t index = std::distance(s_Data.entities.begin(), it);
					const auto& transform = it->GetComponent<TransformComponent>().GetTransform();

					s_Data.treeData[index].Transform = transform;
					UpdateStoredTransform(*it, transform);
				}
			}
		}
	}

	// Existing data generation implementation
	void VoxMemoryAllocator::GenerateData()
	{
		s_Data.treeData.clear();
		s_Data.nodeData.clear();
		s_Data.leafData.clear();
		s_Data.paletteData.clear();

		s_Data.nodeOffset = 0;
		s_Data.leafOffset = 0;

		// Refresh all transform data
		s_Data.lastTransforms.clear();
		s_Data.dirtyEntities.clear();

		for (auto& entity : s_Data.entities) {
			AddTree(entity, s_Data.nodeOffset, s_Data.leafOffset);

			// Refresh stored transform
			const auto& transform = entity.GetComponent<TransformComponent>().GetTransform();
			s_Data.lastTransforms[entity.GetUUID()] = transform;
		}
	}

	void VoxMemoryAllocator::AddTree(Entity& entity, uint32& nodeOffset, uint32& leafOffset)
	{
		SparseVoxelTree tree = entity.GetComponent<VoxelRendererComponent>().SVT;
		GPUSparseVoxelTree gpuTree;

		// Convert Root Node
		GPUSparseVoxelTreeNode gpuRoot;
		gpuRoot.PackedData[0] = (tree.root.IsLeaf << 31) | tree.root.ChildPtr;
		gpuRoot.PackedData[1] = static_cast<uint32_t>(tree.root.ChildMask);
		gpuRoot.PackedData[2] = static_cast<uint32_t>(tree.root.ChildMask >> 32);
		gpuTree.Root = gpuRoot;

		// Set AABB and Transform
		gpuTree.Bounds.Min = Vector4(tree.AABBMin, 0);
		gpuTree.Bounds.Max = Vector4(tree.AABBMax, 0);
		gpuTree.Transform = entity.GetComponent<TransformComponent>().GetTransform();

		// Set NodePool and LeafData pointers
		gpuTree.NodePoolPtr = nodeOffset;
		gpuTree.LeafDataPtr = leafOffset;

		// Set palette
		gpuTree.PaletteDataPtr = s_Data.paletteData.size();

		for (int i = 0; i < 256; i++)
		{
			const auto& color = tree.voxelMap->palette[i];

			s_Data.paletteData.emplace_back(
				color.r / 255.0f,
				color.g / 255.0f,
				color.b / 255.0f,
				1.0f
			);
		}

		// Append to GPU Trees
		s_Data.treeData.push_back(gpuTree);

		// Append Nodes to GPU Pool
		for (const auto& node : tree.nodePool)
		{
			GPUSparseVoxelTreeNode gpuNode;
			gpuNode.PackedData[0] = (node.IsLeaf << 31) | node.ChildPtr;
			gpuNode.PackedData[1] = static_cast<uint32_t>(node.ChildMask);
			gpuNode.PackedData[2] = static_cast<uint32_t>(node.ChildMask >> 32);
			s_Data.nodeData.push_back(gpuNode);
		}

		// Append Leaf Data
		s_Data.leafData.insert(s_Data.leafData.end(), tree.leafData.begin(), tree.leafData.end());

		// Update offsets
		nodeOffset += tree.nodePool.size();
		leafOffset += tree.leafData.size();
	}
}
