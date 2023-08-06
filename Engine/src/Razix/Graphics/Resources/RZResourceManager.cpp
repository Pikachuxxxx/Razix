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
            m_TexturePool.init(128, RZTexture::GetInstanceSize());
            //m_VertexBufferPool.init(128);
            //m_IndexBufferPool.init(128);
        }

        void RZResourceManager::ShutDown()
        {
            RAZIX_CORE_INFO("[Resource Manager] Shutting Down Resource Manager");

            // Destroy all the Pools
            m_TexturePool.destroy();
            //m_VertexBufferPool.destroy();
            //m_IndexBufferPool.destroy();
        }

        RZTextureHandle RZResourceManager::createTexture(const RZTextureDesc& desc)
        {
            // Use the Pool
            return RZTexture::Create(desc RZ_DEBUG_NAME_TAG_STR_E_ARG(desc.name));
        }

        RZTextureHandle RZResourceManager::createTextureFromFile(const char* filePath, const RZTextureDesc& desc)
        {
            return RZTexture::CreateFromFile(filePath, desc RZ_DEBUG_NAME_TAG_STR_E_ARG(desc.name));
        }

        void RZResourceManager::releaseTexture(RZTextureHandle handle)
        {
            // Delete only if it's a valid handle
            if (handle.isValid())
                m_TexturePool.releaseResource(handle.getIndex());
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
