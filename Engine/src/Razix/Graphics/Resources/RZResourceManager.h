#pragma once

#include "Razix/Core/RZHandle.h"

#include "Razix/Graphics/Resources/RZResourcePool.h"

#include "Razix/Graphics/RHI/API/RZAPIDesc.h"
#include "Razix/Graphics/RHI/API/RZAPIHandles.h"

#include "Razix/Utilities/TRZSingleton.h"

namespace Razix {
    namespace Graphics {

#define RAZIX_REGISTER_RESOURCE_POOL(resourceName, ...)                                    \
public:                                                                                    \
    template<>                                                                             \
    RZResourcePoolTyped<RZ##resourceName>& getPool()                                       \
    {                                                                                      \
        return m_##resourceName##Pool;                                                     \
    }                                                                                      \
                                                                                           \
public:                                                                                    \
    RZ##resourceName##Handle create##resourceName(__VA_ARGS__);                            \
    void                     destroy##resourceName(RZ##resourceName##Handle handle);       \
    RZ##resourceName*        get##resourceName##Resource(RZ##resourceName##Handle handle); \
                                                                                           \
private:                                                                                   \
    RZResourcePoolTyped<RZ##resourceName> m_##resourceName##Pool;

        class RZTexture;
        enum class ShaderBuiltin : u32;
        enum class PoolType : u32;

        // TODO: FIX RESOURCE CLEANUP IMMEDIATELY

        /**
         * Resource Manager maintains the CPU and GPU pools for all resource allocated in Razix Engine
         * 
         * API Usage:
         * Resource Creation: get_RESOURCE_TYPE_NAME(args)
         * Resource Destruction: destroy_RESOURCE_TYPE(handle)
         * Resource Get RAW: getRESOURCE_TYPE_Resource(handle)
         *  ex. Draw Command Buffer
         *  RZDrawCommandBufferHandle createDrawCommandBuffer(RZCommandAllocatorPoolHandle pool);
         *  void                      destroyDrawCommandBuffer(RZDrawCommandBufferHandle handle);
         *  RZDrawCommandBuffer*      getDrawCommandBufferResource(RZDrawCommandBufferHandle handle);
         */
        class RAZIX_API RZResourceManager : public RZSingleton<RZResourceManager>
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

            /* Handles Resource Allocation functions */
            //-----------------------------------------------------------------------------------
            RAZIX_REGISTER_RESOURCE_POOL(Texture, const RZTextureDesc& desc)
            //-----------------------------------------------------------------------------------
            RAZIX_REGISTER_RESOURCE_POOL(Shader, const RZShaderDesc& desc)
            //-----------------------------------------------------------------------------------
            RAZIX_REGISTER_RESOURCE_POOL(Pipeline, const RZPipelineDesc& desc)
            //-----------------------------------------------------------------------------------
            RAZIX_REGISTER_RESOURCE_POOL(UniformBuffer, const RZBufferDesc& desc)
            //-----------------------------------------------------------------------------------
            RAZIX_REGISTER_RESOURCE_POOL(CommandPool, PoolType type)
            //-----------------------------------------------------------------------------------
            RAZIX_REGISTER_RESOURCE_POOL(DrawCommandBuffer, RZCommandPoolHandle pool)
            //-----------------------------------------------------------------------------------
        };

#define RZ_GET_RESOURCE_MANAGER()      RZResourceManager::Get()
#define RZ_GET_RAW_RESOURCE(T, handle) RZResourceManager::Get().get##T##Resource(handle)
    }    // namespace Graphics
}    // namespace Razix
