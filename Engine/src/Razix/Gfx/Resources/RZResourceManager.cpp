// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZResourceManager.h"

#include "Razix/Gfx/GfxUtil.h"

#define CREATE_UTIL(name, typeEnum, pool, handleSize)                                                    \
    rz_handle        handle;                                                                             \
    void*            where    = pool.obtain(handle);                                                     \
    rz_gfx_resource* resource = (rz_gfx_resource*) where;                                                \
    if (!where) {                                                                                        \
        RAZIX_CORE_ERROR("[Resource Manager] Failed to create resource: {0}. Pool is full!", name);      \
        return handle;                                                                                   \
    }                                                                                                    \
    resource->type   = typeEnum;                                                                         \
    resource->name   = name;                                                                             \
    resource->handle = handle;                                                                           \
    memcpy(&resource->desc, &desc, handleSize);                                                          \
    if (m_ResourceTypeCBFuncs[typeEnum].createFuncCB) {                                                  \
        m_ResourceTypeCBFuncs[typeEnum].createFuncCB(where);                                             \
    } else {                                                                                             \
        RAZIX_CORE_ERROR("[Resource Manager] Resource Create Callback is NULL for resource: {0}", name); \
    }                                                                                                    \
    return handle;

#define DESTROY_UTIL(pool, message)                                                                                                   \
    if (rz_handle_is_valid(&handle)) {                                                                                                \
        rz_gfx_resource* resource = (rz_gfx_resource*) pool.get(handle);                                                              \
        if (m_ResourceTypeCBFuncs[resource->type].destroyFuncCB) {                                                                    \
            m_ResourceTypeCBFuncs[resource->type].destroyFuncCB((void*) resource);                                                    \
        } else {                                                                                                                      \
            RAZIX_CORE_ERROR("[Resource Manager] Resource Destroy Callback is NULL for resource: {0}", rz_handle_get_index(&handle)); \
        }                                                                                                                             \
        pool.release(handle);                                                                                                         \
    } else                                                                                                                            \
        RAZIX_CORE_ERROR(message);

#define GET_UTIL(pool)               \
    if (rz_handle_is_valid(&handle)) \
        return pool.get(handle);     \
    return nullptr;

#define RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(PoolName, ResourceType, HandleType)                                                                                               \
    HandleType##_handle RZResourceManager::create##PoolName(const char* name, const HandleType##_desc& desc)                                                                 \
    {                                                                                                                                                                        \
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);                                                                                                                  \
        CREATE_UTIL(name, ResourceType, m_##PoolName##Pool, sizeof(HandleType##_desc));                                                                                      \
    }                                                                                                                                                                        \
    void RZResourceManager::destroy##PoolName(HandleType##_handle& handle)                                                                                                   \
    {                                                                                                                                                                        \
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);                                                                                                                  \
        DESTROY_UTIL(m_##PoolName##Pool, std::string("[Resource Manager] Attempting to release ") + std::string(#PoolName) + std::string(" resource with Invalid handle!")); \
    }                                                                                                                                                                        \
    HandleType* RZResourceManager::get##PoolName##Resource(HandleType##_handle handle)                                                                                       \
    {                                                                                                                                                                        \
        GET_UTIL(m_##PoolName##Pool);                                                                                                                                        \
    }

#define RAZIX_INIT_RESOURCE_POOL(PoolName, ResourceType, Capacity, ElementSize, CreateFuncCB, DestroyFuncCB) \
    m_ResourceTypeCBFuncs[ResourceType].createFuncCB  = CreateFuncCB;                                        \
    m_ResourceTypeCBFuncs[ResourceType].destroyFuncCB = DestroyFuncCB;                                       \
    m_##PoolName##Pool.init(Capacity, ElementSize);

#ifdef RAZIX_DEBUG
    #define RAZIX_UNREGISTER_RESOURCE_POOL(PoolName) \
        m_##PoolName##Pool.printResources();         \
        m_##PoolName##Pool.destroy();
#else
    #define RAZIX_UNREGISTER_RESOURCE_POOL(PoolName) \
        m_##PoolName##Pool.destroy();
#endif

namespace Razix {
    namespace Gfx {

        static void RZSFCreateOverrideFunc(void* where)
        {
            rz_gfx_shader_desc* desc = (rz_gfx_shader_desc*) where;
            RAZIX_ASSERT(desc->rzsfFilePath != nullptr, "[Resource Manager] RZSF file path is null! Cannot create shader from RZSF file!");
            rz_gfx_shader_desc parsedDesc = Gfx::ParseRZSF(std::string(desc->rzsfFilePath));
            Memory::RZFree((void*) desc->rzsfFilePath);
            desc->rzsfFilePath = nullptr;
            memcpy(desc, &parsedDesc, sizeof(rz_gfx_shader_desc));
            rzRHI_CreateShader(where);
        }

        void RZResourceManager::StartUp()
        {
            RAZIX_CORE_INFO("[Resource Manager] Starting Up Resource Manager");
            //Razix::RZSplashScreen::Get().setLogString("Starting VFS...");

            // Initialize all the Pools
            //RAZIX_INIT_RESOURCE_POOL(Texture, 2048, sizeof(rz_gfx_texture));
            //RAZIX_INIT_RESOURCE_POOL(Sampler, 32)
            RAZIX_INIT_RESOURCE_POOL(Shader, RZ_GFX_RESOURCE_TYPE_SHADER, 512, sizeof(rz_gfx_shader), RZSFCreateOverrideFunc, rzRHI_DestroyShader);
            //RAZIX_INIT_RESOURCE_POOL(Pipeline, 512)
            //RAZIX_INIT_RESOURCE_POOL(UniformBuffer, 2048)
            RAZIX_INIT_RESOURCE_POOL(CommandPool, RZ_GFX_RESOURCE_TYPE_CMD_POOL, 32, sizeof(rz_gfx_cmdpool), rzRHI_CreateCmdPool, rzRHI_DestroyCmdPool);
            RAZIX_INIT_RESOURCE_POOL(CommandBuffer, RZ_GFX_RESOURCE_TYPE_CMD_BUFFER, 32 * 32, sizeof(rz_gfx_cmdbuf), rzRHI_CreateCmdBuf, rzRHI_DestroyCmdBuf);
            //RAZIX_INIT_RESOURCE_POOL(VertexBuffer, 512)
            //RAZIX_INIT_RESOURCE_POOL(IndexBuffer, 512)
            //RAZIX_INIT_RESOURCE_POOL(DescriptorSet, 128)
        }

        void RZResourceManager::ShutDown()
        {
            RAZIX_CORE_ERROR("[Resource Manager] Shutting Down Resource Manager");

            // Destroy all the Pools
            ////////////////////////////////
            //RAZIX_UNREGISTER_RESOURCE_POOL(Texture)
            //RAZIX_UNREGISTER_RESOURCE_POOL(Sampler)
            RAZIX_UNREGISTER_RESOURCE_POOL(Shader)
            //RAZIX_UNREGISTER_RESOURCE_POOL(Pipeline)
            //RAZIX_UNREGISTER_RESOURCE_POOL(UniformBuffer)
            RAZIX_UNREGISTER_RESOURCE_POOL(CommandPool);
            RAZIX_UNREGISTER_RESOURCE_POOL(CommandBuffer);
            //RAZIX_UNREGISTER_RESOURCE_POOL(VertexBuffer)
            //RAZIX_UNREGISTER_RESOURCE_POOL(IndexBuffer)
            //RAZIX_UNREGISTER_RESOURCE_POOL(DescriptorSet)
            ////////////////////////////////
        }

        //-----------------------------------------------------------------------------------

        //RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(Texture, const RZTextureDesc& desc)
        //RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(Sampler, const RZSamplerDesc& desc)
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(Shader, RZ_GFX_RESOURCE_TYPE_SHADER, rz_gfx_shader)
        //RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(Pipeline, const RZPipelineDesc& desc)
        //RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(UniformBuffer, const RZBufferDesc& desc)
        //RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(VertexBuffer, const RZBufferDesc& desc)
        //RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(IndexBuffer, const RZBufferDesc& desc)
        //RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(DescriptorSet, const RZDescriptorSetDesc& desc)
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(CommandPool, RZ_GFX_RESOURCE_TYPE_CMD_POOL, rz_gfx_cmdpool)
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(CommandBuffer, RZ_GFX_RESOURCE_TYPE_CMD_BUFFER, rz_gfx_cmdbuf)

        //-----------------------------------------------------------------------------------

    }    // namespace Gfx
}    // namespace Razix
