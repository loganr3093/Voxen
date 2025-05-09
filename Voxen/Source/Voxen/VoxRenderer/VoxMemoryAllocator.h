#pragma once  

#include "Voxen/Core/UUID.h"  
#include "Voxen/VoxRenderer/SparseVoxelTree.h"  
#include <glm/glm.hpp>  

#include <unordered_map>  
#include <unordered_set>  
#include <vector>  

namespace Voxen  
{  
   class Entity;  
   struct SparseVoxelTree;  
   struct GPUSparseVoxelTree;  
   struct GPUSparseVoxelTreeNode;  

   class VoxMemoryAllocator  
   {  
   public:  
       VoxMemoryAllocator() = default;  
       ~VoxMemoryAllocator() = default;  

       // Allocation  
       void Allocate(Entity& entity);  
       void Deallocate(Entity& entity);  
       size_t Count() const;  
       void Clear();  

       // Transform tracking API  
       bool HasTransformChanged(Entity& entity, const glm::mat4& currentTransform) const;  
       void MarkDirty(Entity& entity);  
       void UpdateStoredTransform(Entity& entity, const glm::mat4& transform);  

       // Dirty state checks  
       bool IsStructureDirty() const;  
       bool IsDataDirty() const;  
       void Flush();  
       void Refresh();  

       // New method to mark structure dirty  
       void MarkStructureDirty() { m_isStructureDirty = true; }
	   void MarkDataDirty() { m_isDataDirty = true; }

       // Data access  
       const std::vector<GPUSparseVoxelTree>& GetTreeData();  
       const std::vector<GPUSparseVoxelTreeNode>& GetNodeData();  
       const std::vector<uint32>& GetLeafData();  
       const std::vector<Vector4>& GetPaletteData();  

       uint8 GetCurrentPaletteSize() const;  

   private:  
       // GPU data generation  
       void GenerateData();  
       void AddTree(Entity& entity, uint32& nodeOffset, uint32& leafOffset);  

       // Member data  
       std::vector<Entity> m_entities;  
       bool m_isStructureDirty = false;  
       bool m_isDataDirty = false;  

       // Transform tracking  
       std::unordered_map<UUID, glm::mat4> m_lastTransforms;  
       std::unordered_set<UUID> m_dirtyEntities;  

       // GPU buffers  
       std::vector<GPUSparseVoxelTree> m_treeData;  
       std::vector<GPUSparseVoxelTreeNode> m_nodeData;  
       std::vector<uint32> m_leafData;  
       std::vector<Vector4> m_paletteData;  

       // Offsets  
       uint32 m_nodeOffset = 0;  
       uint32 m_leafOffset = 0;  
   };  
}