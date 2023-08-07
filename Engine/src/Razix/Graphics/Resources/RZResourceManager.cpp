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
            RZHandle<RZTexture> handle;
            void*               where = m_TexturePool.obtain(handle);
            RZTexture::Create(where, desc RZ_DEBUG_NAME_TAG_STR_E_ARG(desc.name));
            IRZResource<RZTexture>* resource = (IRZResource<RZTexture>*) where;
            resource->setHandle(handle);
            return handle;
        }

        RZTextureHandle RZResourceManager::createTextureFromFile(const RZTextureDesc& desc, const std::string& filePath)
        {
            RZHandle<RZTexture> handle;
            void*               where = m_TexturePool.obtain(handle);
            RZTexture::CreateFromFile(where, desc, filePath RZ_DEBUG_NAME_TAG_STR_E_ARG(desc.name));
            IRZResource<RZTexture>* resource = (IRZResource<RZTexture>*) where;
            resource->setHandle(handle);
            return handle;
        }

        void RZResourceManager::releaseTexture(RZTextureHandle& handle)
        {
            // Delete only if it's a valid handle, else skip it and report it
            if (handle.isValid())
                m_TexturePool.release(handle);
            else
                RAZIX_CORE_ERROR("[Resource Manager] Attempting to release a resource with Invalid handle!");
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
