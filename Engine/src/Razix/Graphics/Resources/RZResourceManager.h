#pragma once

#include "Razix/Core/RZHandle.h"

#include "Razix/Graphics/Resources/RZResourcePool.h"

#include "Razix/Graphics/RHI/API/RZAPIDesc.h"
#include "Razix/Graphics/RHI/API/RZAPIHandles.h"

#include "Razix/Utilities/TRZSingleton.h"

namespace Razix {
    namespace Graphics {

        class RZTexture;
        enum class ShaderBuiltin : u32;

        class RAZIX_API RZResourceManager : public RZSingleton<RZResourceManager>
        {
        public:
            /* Initializes the Resource System */
            void StartUp();
            /* Shuts down the Resource System */
            void ShutDown();

            // TODO: Maybe make these private and use them via friend class?
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
            RZTextureHandle createTextureFromFile(const RZTextureDesc& desc, const std::string& filePath);
            void            releaseTexture(RZTextureHandle& handle);

            RZShaderHandle createShaderFromFile(ShaderBuiltin shaderID, std::string shaderPath);
            void           destroyShader(RZShaderHandle& handle);

            RZVertexBufferHandle createVertexBuffer(RZVertexBufferDesc& desc);
            RZIndexBufferHandle  createIndexBuffer(RZIndexBufferDesc& desc);

        private:
            RZResourcePoolTyped<RZTexture>      m_TexturePool;
            RZResourcePoolTyped<RZShader>       m_ShaderPool;
            RZResourcePoolTyped<RZVertexBuffer> m_VertexBufferPool;
            RZResourcePoolTyped<RZIndexBuffer>  m_IndexBufferPool;
        };
    }    // namespace Graphics
}    // namespace Razix
