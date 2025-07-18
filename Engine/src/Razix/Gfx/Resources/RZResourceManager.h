#pragma once

#include "Razix/Core/RZHandle.h"

#include "Razix/Gfx/Resources/RZResourceFreeListMemPool.h"

#include "Razix/Utilities/TRZSingleton.h"

#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {
    namespace Gfx {

        struct RZResourceCBFuncs
        {
            rz_gfx_resource_create_fn  createFuncCB;
            rz_gfx_resource_destroy_fn destroyFuncCB;
        };

#define RAZIX_REGISTER_RESOURCE_POOL(poolName, resourceTypeName)                                       \
public:                                                                                                \
    template<>                                                                                         \
    RZResourceFreeListMemPoolTyped<resourceTypeName>& getPool()                                        \
    {                                                                                                  \
        return m_##poolName##Pool;                                                                     \
    }                                                                                                  \
                                                                                                       \
public:                                                                                                \
    resourceTypeName##_handle create##poolName(const char* name, const resourceTypeName##_desc& desc); \
    void                      destroy##poolName(resourceTypeName##_handle& handle);                    \
    resourceTypeName*         get##poolName##Resource(resourceTypeName##_handle handle);               \
                                                                                                       \
private:                                                                                               \
    RZResourceFreeListMemPoolTyped<resourceTypeName> m_##poolName##Pool;

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
            RZResourceFreeListMemPoolTyped<T>& getPool()
            {
            }

            /* Handles Resource Allocation functions */
            //-----------------------------------------------------------------------------------
            RAZIX_REGISTER_RESOURCE_POOL(Texture, rz_gfx_texture)
            // //-----------------------------------------------------------------------------------
            // RAZIX_REGISTER_RESOURCE_POOL(sampler, const RZSamplerDesc& desc)
            // //-----------------------------------------------------------------------------------
            RAZIX_REGISTER_RESOURCE_POOL(Shader, rz_gfx_shader)
            // //-----------------------------------------------------------------------------------
            // RAZIX_REGISTER_RESOURCE_POOL(pipeline, const RZPipelineDesc& desc)
            // //-----------------------------------------------------------------------------------
            // RAZIX_REGISTER_RESOURCE_POOL(uniformBuffer, const RZBufferDesc& desc)
            // //-----------------------------------------------------------------------------------
            // RAZIX_REGISTER_RESOURCE_POOL(vertex_buffer, const RZBufferDesc& desc)
            // //-----------------------------------------------------------------------------------
            // RAZIX_REGISTER_RESOURCE_POOL(index_buffer, const RZBufferDesc& desc)
            //
            //-----------------------------------------------------------------------------------
            RAZIX_REGISTER_RESOURCE_POOL(CommandPool, rz_gfx_cmdpool)
            //-----------------------------------------------------------------------------------
            RAZIX_REGISTER_RESOURCE_POOL(CommandBuffer, rz_gfx_cmdbuf)
            //-----------------------------------------------------------------------------------
            // RAZIX_REGISTER_RESOURCE_POOL(DescriptorSet, const RZDescriptorSetDesc& desc)
            // //-----------------------------------------------------------------------------------

        private:
            RZResourceCBFuncs m_ResourceTypeCBFuncs[RZ_GFX_RESOURCE_TYPE_COUNT];
        };

#define RZ_GET_RESOURCE_MANAGER()      RZResourceManager::Get()
#define RZ_GET_RAW_RESOURCE(T, handle) RZResourceManager::Get().get##T##Resource(handle)
    }    // namespace Gfx
}    // namespace Razix
