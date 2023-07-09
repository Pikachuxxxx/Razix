#pragma once

#include "Razix/Core/RZHandle.h"

#include "Razix/Graphics/RHI/API/RZAPICreateStructs.h"
#include "Razix/Graphics/RHI/API/RZAPIHandles.h"

#include "Razix/Graphics/Resources/RZResourcePool.h"

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
                if (typeid(T) == typeid(RZTexture2D))
                    return m_Texture2DPool;
            }

            /* GPU Resource Allocation functions */
            RZTexture2DHandle    createTexture2D(RZTextureDesc& desc);
            RZVertexBufferHandle createVertexBuffer(RZVertexBufferDesc& desc);
            RZIndexBufferHandle  createIndexBuffer(RZIndexBufferDesc& desc);

        private:
            RZResourcePoolTyped<RZTexture2D>    m_Texture2DPool;
            RZResourcePoolTyped<RZVertexBuffer> m_VertexBufferPool;
            RZResourcePoolTyped<RZIndexBuffer>  m_IndexBufferPool;
        };
    }    // namespace Graphics
}    // namespace Razix
