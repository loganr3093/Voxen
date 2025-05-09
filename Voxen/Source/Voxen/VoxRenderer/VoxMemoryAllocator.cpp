#include "voxpch.h"
#include "Voxen/VoxRenderer/VoxMemoryAllocator.h"
#include "Voxen/VoxRenderer/SparseVoxelTree.h"

#include "ogt/vox.h"

#include "Voxen/Scene/Entity.h"
#include "Voxen/Scene/Components.h"
#include <bitset>
#include <algorithm>

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

	void VoxMemoryAllocator::Allocate(Entity& entity)
	{
		m_entities.push_back(entity);
		m_isStructureDirty = true;
		m_lastTransforms[entity.GetUUID()] = entity.GetComponent<TransformComponent>().GetTransform();
	}

	void VoxMemoryAllocator::Deallocate(Entity& entity)
	{
		auto it = std::find(m_entities.begin(), m_entities.end(), entity);
		if (it == m_entities.end()) return;
		UUID id = entity.GetUUID();
		m_lastTransforms.erase(id);
		m_dirtyEntities.erase(id);
		std::swap(*it, m_entities.back());
		m_entities.pop_back();
		m_isStructureDirty = true;
	}

	size_t VoxMemoryAllocator::Count() const
	{
		return m_entities.size();
	}

	void VoxMemoryAllocator::Clear()
	{
		m_entities.clear();
		m_isStructureDirty = true;
		m_isDataDirty = true;
		m_lastTransforms.clear();
		m_dirtyEntities.clear();
		m_treeData.clear();
		m_nodeData.clear();
		m_leafData.clear();
		m_paletteData.clear();
		m_nodeOffset = 0;
		m_leafOffset = 0;
	}

	bool VoxMemoryAllocator::HasTransformChanged(Entity& entity, const glm::mat4& currentTransform) const {
		auto it = m_lastTransforms.find(entity.GetUUID());
		return it != m_lastTransforms.end() && it->second != currentTransform;
	}

	void VoxMemoryAllocator::MarkDirty(Entity& entity) {
		m_dirtyEntities.insert(entity.GetUUID());
		m_isDataDirty = true;
	}

	void VoxMemoryAllocator::UpdateStoredTransform(Entity& entity, const glm::mat4& transform) {
		m_lastTransforms[entity.GetUUID()] = transform;
	}

	bool VoxMemoryAllocator::IsStructureDirty() const { return m_isStructureDirty; }
	bool VoxMemoryAllocator::IsDataDirty() const { return m_isDataDirty; }

	const std::vector<GPUSparseVoxelTree>& VoxMemoryAllocator::GetTreeData() {
		Flush();
		return m_treeData;
	}

	const std::vector<GPUSparseVoxelTreeNode>& VoxMemoryAllocator::GetNodeData() {
		Flush();
		return m_nodeData;
	}

	const std::vector<uint32>& VoxMemoryAllocator::GetLeafData() {
		Flush();
		return m_leafData;
	}

	const std::vector<Vector4>& VoxMemoryAllocator::GetPaletteData() {
		Flush();
		return m_paletteData;
	}

	uint8 VoxMemoryAllocator::GetCurrentPaletteSize() const {
		return static_cast<uint8>(m_paletteData.size());
	}

	void VoxMemoryAllocator::Flush() {
		if (m_isStructureDirty) {
			GenerateData();
			m_isStructureDirty = false;
			m_isDataDirty = false;
		}
		else if (m_isDataDirty) {
			// Partial transform updates
			for (auto& uuid : m_dirtyEntities) {
				auto it = std::find_if(m_entities.begin(), m_entities.end(),
					[&](Entity& e) { return e.GetUUID() == uuid; });
				if (it != m_entities.end()) {
					size_t idx = std::distance(m_entities.begin(), it);
					auto t = it->GetComponent<TransformComponent>().GetTransform();
					m_treeData[idx].Transform = t;
					UpdateStoredTransform(*it, t);
				}
			}
			m_dirtyEntities.clear();
			m_isDataDirty = false;
		}
	}

	void VoxMemoryAllocator::Refresh() {
		if (m_isStructureDirty)
			GenerateData();
		// on data dirty we rely on Flush() partial updates
	}

	void VoxMemoryAllocator::GenerateData() {
		m_treeData.clear();
		m_nodeData.clear();
		m_leafData.clear();
		m_paletteData.clear();
		m_nodeOffset = 0;
		m_leafOffset = 0;
		m_lastTransforms.clear();
		m_dirtyEntities.clear();

		for (auto& entity : m_entities)
		{
			AddTree(entity, m_nodeOffset, m_leafOffset);
			m_lastTransforms[entity.GetUUID()] = entity.GetComponent<TransformComponent>().GetTransform();
		}
	}

	void VoxMemoryAllocator::AddTree(Entity& entity, uint32& nodeOffset, uint32& leafOffset) {
		auto& comp = entity.GetComponent<VoxelRendererComponent>();
		SparseVoxelTree& tree = const_cast<SparseVoxelTree&>(comp.SVT);
		GPUSparseVoxelTree gpuTree;

		// Root node
		GPUSparseVoxelTreeNode rootNode;
		rootNode.PackedData[0] = (tree.root.IsLeaf << 31) | tree.root.ChildPtr;
		rootNode.PackedData[1] = static_cast<uint32>(tree.root.ChildMask);
		rootNode.PackedData[2] = static_cast<uint32>(tree.root.ChildMask >> 32);
		gpuTree.Root = rootNode;

		// Bounds & transform
		gpuTree.Bounds.Min = Vector4(tree.AABBMin, 0.0f);
		gpuTree.Bounds.Max = Vector4(tree.AABBMax, 0.0f);
		gpuTree.Transform = entity.GetComponent<TransformComponent>().GetTransform();

		// Offsets
		gpuTree.NodePoolPtr = nodeOffset;
		gpuTree.LeafDataPtr = leafOffset;
		gpuTree.PaletteDataPtr = static_cast<uint32>(m_paletteData.size());

		// Palette
		for (int i = 0; i < 256; ++i) {
			auto& c = tree.voxelMap->palette[i];
			m_paletteData.emplace_back(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, 1.0f);
		}

		// ID & scale
		gpuTree.EntityID = (uint32)entity;
		gpuTree.InitialScale = tree.initialScale;
		m_treeData.push_back(gpuTree);

		// Nodes
		for (auto& n : tree.nodePool) {
			GPUSparseVoxelTreeNode node;
			node.PackedData[0] = (n.IsLeaf << 31) | n.ChildPtr;
			node.PackedData[1] = static_cast<uint32>(n.ChildMask);
			node.PackedData[2] = static_cast<uint32>(n.ChildMask >> 32);
			m_nodeData.push_back(node);
		}

		// Leaves
		m_leafData.insert(m_leafData.end(), tree.leafData.begin(), tree.leafData.end());

		nodeOffset += static_cast<uint32>(tree.nodePool.size());
		leafOffset += static_cast<uint32>(tree.leafData.size());
	}
}
