#pragma once

#include "Razix/Core/RZHandle.h"

#include "Razix/Graphics/Resources/RZResourcePool.h"

#include "Razix/Graphics/RHI/API/RZAPICreateStructs.h"
#include "Razix/Graphics/RHI/API/RZAPIHandles.h"

#include "Razix/Utilities/TRZSingleton.h"

namespace Razix {
    namespace Graphics {

        class RZTexture2D;

        class RZResourceManager : public RZSingleton<RZResourceManager>
        {
        public:
            /* Initializes the Resource System */
            void StartUp();
            /* Shuts down the Resource System */
            void ShutDown();

            template<class T>
            RZResourcePoolTyped<T>& getPool()
            {
            }

            template<>
            RZResourcePoolTyped<RZTexture>& getPool()
            {
                return m_TexturePool;
            }

            /* GPU Resource Allocation functions */
            RZTextureHandle createTexture(const RZTextureDesc& desc);
            RZTextureHandle createTextureFromFile(const char* filePath, const RZTextureDesc& desc);
            void            releaseTexture(RZTextureHandle handle);

            RZVertexBufferHandle createVertexBuffer(RZVertexBufferDesc& desc);
            RZIndexBufferHandle  createIndexBuffer(RZIndexBufferDesc& desc);

        private:
            RZResourcePoolTyped<RZTexture>      m_TexturePool;
            RZResourcePoolTyped<RZVertexBuffer> m_VertexBufferPool;
            RZResourcePoolTyped<RZIndexBuffer>  m_IndexBufferPool;
        };
    }    // namespace Graphics
}    // namespace Razix
