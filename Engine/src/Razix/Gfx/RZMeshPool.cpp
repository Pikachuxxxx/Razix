// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZMeshPool.h"

namespace Razix {
    namespace Gfx {

        void RZMeshPool::init(u32 poolSize)
        {
            m_MeshPool.init(poolSize, sizeof(RZMesh));
        }

        void RZMeshPool::destroy()
        {
            m_MeshPool.destroy();
        }

        void RZMeshPool::deallocate(rz_mesh_handle& handle)
        {
            RZMesh* mesh = m_MeshPool.get(handle);
            mesh->Destroy();
            mesh->~RZMesh();
            m_MeshPool.release(handle);
        }

        RZMesh* RZMeshPool::getMeshData(rz_mesh_handle handle)
        {
            return m_MeshPool.get(handle);
        }
    }    // namespace Gfx
}    // namespace Razix
