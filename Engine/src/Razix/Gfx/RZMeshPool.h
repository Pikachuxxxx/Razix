#pragma once

#include "Razix/Gfx/RZMesh.h"

#include "Razix/Gfx/Resources/RZResourceFreeListMemPool.h"

namespace Razix {
    namespace Gfx {

        using rz_mesh_handle = rz_handle;

        class RAZIX_API RZMeshPool
        {
        public:
            RZMeshPool()  = default;
            ~RZMeshPool() = default;

            void init(u32 poolSize);
            void destroy();

            template<typename... Args>
            rz_mesh_handle allocate(Args&&... args)
            {
                rz_mesh_handle handle{};
                RZMesh*        where = m_MeshPool.obtain(handle);
                new (where) RZMesh(std::forward<Args>(args)...);
                return handle;
            }
            void    deallocate(rz_mesh_handle& handle);
            RZMesh* getMeshData(rz_mesh_handle handle);

        private:
            RZResourceFreeListMemPoolTyped<RZMesh> m_MeshPool;
        };
    }    // namespace Gfx
}    // namespace Razix
