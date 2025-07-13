#pragma once

#include "Razix/Core/RZHandle.h"

#include "Razix/Gfx/Resources/RZResourcePool.h"

#include "Razix/Utilities/TRZSingleton.h"

#include "Razix/Gfx/GfxData.h"

namespace Razix {
    namespace Gfx {

#define RAZIX_REGISTER_RESOURCE_POOL(resourceName, ...)                                         \
public:                                                                                         \
    template<>                                                                                  \
    RZResourcePoolTyped<rz_gfx_##resourceName##>& getPool()                                     \
    {                                                                                           \
        return m_##resourceName##Pool;                                                          \
    }                                                                                           \
                                                                                                \
public:                                                                                         \
    rz_##resourceName##_handle create_##resourceName(__VA_ARGS__);                              \
    void                       destroy_##resourceName(rz_##resourceName##_handle& handle);      \
    rz_gfx_##resourceName*     get_##resourceName##Resource(rz_##resourceName##_handle handle); \
                                                                                                \
private:                                                                                        \
    RZResourcePoolTyped<rz_gfx_##resourceName> m_##resourceName##Pool;

        struct rz_gfx_texture;
        enum ShaderBuiltin;
        enum rz_gfx_pool_type;

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
            RAZIX_REGISTER_RESOURCE_POOL(texture, const rz_texture_desc& desc)
            // //-----------------------------------------------------------------------------------
            // RAZIX_REGISTER_RESOURCE_POOL(sampler, const RZSamplerDesc& desc)
            // //-----------------------------------------------------------------------------------
            // RAZIX_REGISTER_RESOURCE_POOL(shader, const RZShaderDesc& desc)
            // //-----------------------------------------------------------------------------------
            // RAZIX_REGISTER_RESOURCE_POOL(pipeline, const RZPipelineDesc& desc)
            // //-----------------------------------------------------------------------------------
            // RAZIX_REGISTER_RESOURCE_POOL(uniformBuffer, const RZBufferDesc& desc)
            // //-----------------------------------------------------------------------------------
            // RAZIX_REGISTER_RESOURCE_POOL(vertex_buffer, const RZBufferDesc& desc)
            // //-----------------------------------------------------------------------------------
            // RAZIX_REGISTER_RESOURCE_POOL(index_buffer, const RZBufferDesc& desc)
            //
            // //-----------------------------------------------------------------------------------
            // RAZIX_REGISTER_RESOURCE_POOL(command_pool, PoolType type)
            // //-----------------------------------------------------------------------------------
            // RAZIX_REGISTER_RESOURCE_POOL(DrawCommandBuffer, RZCommandPoolHandle pool)
            // //-----------------------------------------------------------------------------------
            // RAZIX_REGISTER_RESOURCE_POOL(DescriptorSet, const RZDescriptorSetDesc& desc)
            // //-----------------------------------------------------------------------------------
        };

#define RZ_GET_RESOURCE_MANAGER()      RZResourceManager::Get()
#define RZ_GET_RAW_RESOURCE(T, handle) RZResourceManager::Get().get##T##Resource(handle)
    }    // namespace Gfx
}    // namespace Razix
