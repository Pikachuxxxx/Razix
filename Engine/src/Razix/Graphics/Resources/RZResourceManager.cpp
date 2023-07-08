// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZResourceManager.h"

#include "Razix/Graphics/RHI/API/RZTexture.h"

namespace Razix {
    namespace Graphics {

        void RZResourceManager::StartUp()
        {
            RAZIX_CORE_INFO("[Resource Manager] Starting Up Resource Manager");

            // TODO: Use static methods on resource to calculate the actual size of the underlying API class implementations
            // Initialize all the Pools
            m_Texture2DPool.init(128, sizeof(RZTexture2D));
            //m_VertexBufferPool.init(128);
            //m_IndexBufferPool.init(128);
        }

        void RZResourceManager::ShutDown()
        {
            RAZIX_CORE_INFO("[Resource Manager] Shutting Down Resource Manager");

            // Destroy all the Pools
            m_Texture2DPool.destroy();
            //m_VertexBufferPool.destroy();
            //m_IndexBufferPool.destroy();
        }

        RZTexture2DHandle RZResourceManager::createTexture2D(RZTextureDesc& desc)
        {
            // Use the Pool
            return RZTexture2DHandle();
        }

        RZVertexBufferHandle RZResourceManager::createVertexBuffer(RZVertexBufferDesc& desc)
        {
            return RZVertexBufferHandle();
        }

        RZIndexBufferHandle RZResourceManager::createIndexBuffer(RZIndexBufferDesc& desc)
        {
            return RZIndexBufferHandle();
        }
    }    // namespace Graphics
}    // namespace Razix
