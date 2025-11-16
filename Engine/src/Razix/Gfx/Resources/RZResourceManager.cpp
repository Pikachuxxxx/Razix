// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZResourceManager.h"

#include "Razix/Core/std/sprintf.h"
#include "Razix/Gfx/RZGfxUtil.h"

// use the hot data handle to get the cold data pool
#define CREATE_UTIL(name, typeEnum, pool, cold_pool, handleSize)                                                  \
    rz_handle        handle;                                                                                      \
    void*            where    = pool.obtain(handle);                                                              \
    rz_gfx_resource* resource = (rz_gfx_resource*) where;                                                         \
    if (!where) {                                                                                                 \
        RAZIX_CORE_ERROR("[Resource Manager] Failed to create resource: {0}. Pool is full!", name);               \
        return handle;                                                                                            \
    }                                                                                                             \
    memset(resource, 0x00, sizeof(rz_gfx_resource));                                                              \
    resource->hot.type   = typeEnum;                                                                              \
    resource->hot.handle = handle;                                                                                \
    resource->pCold      = cold_pool.get(resource->hot.handle);                                                   \
    if (!resource->pCold) {                                                                                       \
        RAZIX_CORE_ERROR("[Resource Manager] Failed to create cold data for resource: {0}. Pool is full!", name); \
        pool.release(handle);                                                                                     \
        rz_handle_destroy(&handle);                                                                               \
        return handle;                                                                                            \
    }                                                                                                             \
    rz_snprintf(resource->pCold->pName, 256, "%s", name);                                                         \
    memcpy(&resource->pCold->desc, &desc, handleSize);                                                            \
    if (m_ResourceTypeCBFuncs[typeEnum].createFuncCB) {                                                           \
        m_ResourceTypeCBFuncs[typeEnum].createFuncCB(where);                                                      \
    } else {                                                                                                      \
        RAZIX_CORE_ERROR("[Resource Manager] Resource Create Callback is NULL for resource: {0}", name);          \
    }                                                                                                             \
    return handle;

#define DESTROY_UTIL(pool, cold_pool, message)                                                                                        \
    if (rz_handle_is_valid(&handle)) {                                                                                                \
        rz_gfx_resource* resource = (rz_gfx_resource*) pool.get(handle);                                                              \
        RAZIX_ASSERT(resource != NULL, "[Resource Manager] Resource is NULL! Cannot destroy resource!");                              \
        if (m_ResourceTypeCBFuncs[resource->hot.type].destroyFuncCB) {                                                                \
            m_ResourceTypeCBFuncs[resource->hot.type].destroyFuncCB((void*) resource);                                                \
        } else {                                                                                                                      \
            RAZIX_CORE_ERROR("[Resource Manager] Resource Destroy Callback is NULL for resource: {0}", rz_handle_get_index(&handle)); \
        }                                                                                                                             \
        pool.release(handle);                                                                                                         \
        cold_pool.release(handle);                                                                                                    \
        rz_handle_destroy(&handle);                                                                                                   \
    } else                                                                                                                            \
        RAZIX_CORE_ERROR(message);

#define GET_UTIL(pool)               \
    if (rz_handle_is_valid(&handle)) \
        return pool.get(handle);     \
    return NULL;

#define RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(PoolName, ResourceType, HandleType)                                                                                                                  \
    HandleType##_handle RZResourceManager::create##PoolName(const char* name, const HandleType##_desc& desc)                                                                                    \
    {                                                                                                                                                                                           \
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);                                                                                                                                     \
        CREATE_UTIL(name, ResourceType, m_##PoolName##Pool, m_##PoolName##ColdDataPool, sizeof(HandleType##_desc));                                                                             \
    }                                                                                                                                                                                           \
    void RZResourceManager::destroy##PoolName(HandleType##_handle& handle)                                                                                                                      \
    {                                                                                                                                                                                           \
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);                                                                                                                                     \
        DESTROY_UTIL(m_##PoolName##Pool, m_##PoolName##ColdDataPool, RZString("[Resource Manager] Attempting to release ") + RZString(#PoolName) + RZString(" resource with Invalid handle!")); \
    }                                                                                                                                                                                           \
    HandleType* RZResourceManager::get##PoolName##Resource(HandleType##_handle handle)                                                                                                          \
    {                                                                                                                                                                                           \
        GET_UTIL(m_##PoolName##Pool);                                                                                                                                                           \
    }

#define RAZIX_INIT_RESOURCE_POOL(PoolName, ResourceType, Capacity, ElementSize, CreateFuncCB, DestroyFuncCB) \
    m_ResourceTypeCBFuncs[ResourceType].createFuncCB  = CreateFuncCB;                                        \
    m_ResourceTypeCBFuncs[ResourceType].destroyFuncCB = DestroyFuncCB;                                       \
    m_##PoolName##Pool.init(Capacity, ElementSize);                                                          \
    m_##PoolName##ColdDataPool.init(Capacity, ElementSize);

#ifdef RAZIX_DEBUG
    #define RAZIX_UNREGISTER_RESOURCE_POOL(PoolName)                                     \
        RAZIX_CORE_INFO("[Resource Manager] Destroying " #PoolName " Resource Pool..."); \
        m_##PoolName##Pool.printResources();                                             \
        m_##PoolName##Pool.destroy();                                                    \
        m_##PoolName##ColdDataPool.printResources();                                     \
        m_##PoolName##ColdDataPool.destroy();
#else
    #define RAZIX_UNREGISTER_RESOURCE_POOL(PoolName) \
        m_##PoolName##Pool.destroy();                \
        m_##PoolName##ColdDataPool.destroy();
#endif

namespace Razix {
    namespace Gfx {

        static void RZSFCreateOverrideFunc(void* where)
        {
            // This function is called when a shader resource is created from an RZSF file
            rz_gfx_shader* shader = (rz_gfx_shader*) where;
            RAZIX_ASSERT(shader != NULL, "[Resource Manager] Shader resource is null! Cannot create shader from RZSF file!");

            // Get the shader description from the shader resource
            rz_gfx_shader_desc* shaderDesc = &shader->resource.pCold->desc.shaderDesc;
            RAZIX_ASSERT(shaderDesc != NULL, "[Resource Manager] Shader description is null! Cannot create shader from RZSF file!");
            RAZIX_ASSERT(shaderDesc->rzsfFilePath != NULL, "[Resource Manager] RZSF file path is null! Cannot create shader from RZSF file!");

            // Parse the RZSF file and fill the shaderDesc with the parsed data
            rz_gfx_shader_desc parsedDesc = Gfx::ParseRZSF(RZString(shaderDesc->rzsfFilePath));
            shaderDesc->rzsfFilePath      = NULL;

            // Copy the parsed description to the shaderDesc
            memcpy(shaderDesc, &parsedDesc, sizeof(rz_gfx_shader_desc));
            // Create the shader using the RHI
            rzRHI_CreateShader(where);

            // Reflect the shader resource to get the shader reflection data
            rz_gfx_shader_reflection   reflection  = Gfx::ReflectShader(shader);
            rz_gfx_root_signature_desc rootSigDesc = {};
            Gfx::CopyReflectedRootSigDesc(&reflection, &rootSigDesc);

            // Also copy the input elements
            if (reflection.elementCount > 0) {
                Gfx::CopyReflectedInputElements(&reflection, &shaderDesc->pElements, &shaderDesc->elementsCount);
            } else {
                shaderDesc->pElements     = NULL;
                shaderDesc->elementsCount = 0;
            }

            // Or we can cache the reflection data in the shader resource itself
            // Or in a BindMap and also store the descriptor table information and all this

            // create root signature
            if (!(shaderDesc->flags & RZ_GFX_SHADER_FLAG_NO_ROOT_SIGNATURE)) {
#ifdef RAZIX_DEBUG
                if (rootSigDesc.descriptorTableLayoutsCount == 0 && rootSigDesc.pRootConstantsDesc == NULL)
                    RAZIX_CORE_WARN("[Resource Manager] Empty Root Signature! Shader {0} has no root signature descriptor tables or root constants!", shader->resource.pCold->pName);
#endif
                auto rootSigName      = "RootSignature." + RZString(shader->resource.pCold->pName);
                shader->rootSignature = RZResourceManager::Get().createRootSignature(rootSigName.c_str(), rootSigDesc);
            }

            // Any intermediate memory allocations made by the reflection process should be freed
            // But this will also delete some of the root sig desc data
            Gfx::FreeShaderReflectionMemAllocs(&reflection);
        }

        static void DestroyShaderWithRootSigOverrideFunv(void* ptr)
        {
            rz_gfx_shader* shaderPtr = (rz_gfx_shader*) ptr;
            // Free bytecode memory, since we allocate it
            FreeRZSFBytecodeAlloc(shaderPtr);
            rz_gfx_root_signature* rootSig = RZResourceManager::Get().getRootSignatureResource(shaderPtr->rootSignature);
            FreeRootSigDescMemAllocs(&rootSig->resource.pCold->desc.rootSignatureDesc);
            FreeInputElementsMemAllocs(shaderPtr->resource.pCold->desc.shaderDesc.pElements, shaderPtr->resource.pCold->desc.shaderDesc.elementsCount);
            // Free root sig (since it's allocated by ParseRZSF we ask it to delete it)
            if (!(shaderPtr->resource.pCold->desc.shaderDesc.flags & RZ_GFX_SHADER_FLAG_NO_ROOT_SIGNATURE))
                RZResourceManager::Get().destroyRootSignature(shaderPtr->rootSignature);
            rzRHI_DestroyShader(ptr);
        }

        //-----------------------------------------------------------------------------------

        void RZResourceManager::StartUp()
        {
            RAZIX_CORE_INFO("[Resource Manager] Starting Up Resource Manager");
            //Razix::RZSplashScreen::Get().setLogString("Starting Resource Manager...");

            // ~ 10MiB for Resource View Pool (160 bytes * 65536)
            RAZIX_INIT_RESOURCE_POOL(ResourceView, RZ_GFX_RESOURCE_TYPE_RESOURCE_VIEW, 65536, sizeof(rz_gfx_resource_view), rzRHI_CreateResourceView, rzRHI_DestroyResourceView);

            // Initialize all the Pools
            // clang-format off
            // resource name | type | count | element size | create and destroy function pointers
            RAZIX_INIT_RESOURCE_POOL(Texture,         RZ_GFX_RESOURCE_TYPE_TEXTURE,          2048,        sizeof(rz_gfx_texture),            rzRHI_CreateTexture,         rzRHI_DestroyTexture);
            RAZIX_INIT_RESOURCE_POOL(Sampler,         RZ_GFX_RESOURCE_TYPE_SAMPLER,          64,          sizeof(rz_gfx_sampler),            rzRHI_CreateSampler,         rzRHI_DestroySampler);
            RAZIX_INIT_RESOURCE_POOL(Shader,          RZ_GFX_RESOURCE_TYPE_SHADER,           512,         sizeof(rz_gfx_shader),             RZSFCreateOverrideFunc,      DestroyShaderWithRootSigOverrideFunv);
            RAZIX_INIT_RESOURCE_POOL(RootSignature,   RZ_GFX_RESOURCE_TYPE_ROOT_SIGNATURE,   512,         sizeof(rz_gfx_root_signature),     rzRHI_CreateRootSignature,   rzRHI_DestroyRootSignature);
            RAZIX_INIT_RESOURCE_POOL(Pipeline,        RZ_GFX_RESOURCE_TYPE_PIPELINE,         512,         sizeof(rz_gfx_pipeline),           rzRHI_CreatePipeline,        rzRHI_DestroyPipeline);
            RAZIX_INIT_RESOURCE_POOL(Buffer,          RZ_GFX_RESOURCE_TYPE_BUFFER,           4096,        sizeof(rz_gfx_buffer),             rzRHI_CreateBuffer,          rzRHI_DestroyBuffer);
            RAZIX_INIT_RESOURCE_POOL(CommandPool,     RZ_GFX_RESOURCE_TYPE_CMD_POOL,         32,          sizeof(rz_gfx_cmdpool),            rzRHI_CreateCmdPool,         rzRHI_DestroyCmdPool);
            RAZIX_INIT_RESOURCE_POOL(CommandBuffer,   RZ_GFX_RESOURCE_TYPE_CMD_BUFFER,       32 * 32,     sizeof(rz_gfx_cmdbuf),             rzRHI_CreateCmdBuf,          rzRHI_DestroyCmdBuf);
            RAZIX_INIT_RESOURCE_POOL(DescriptorHeap,  RZ_GFX_RESOURCE_TYPE_DESCRIPTOR_HEAP,  4096,        sizeof(rz_gfx_descriptor_heap),    rzRHI_CreateDescriptorHeap,  rzRHI_DestroyDescriptorHeap);
            RAZIX_INIT_RESOURCE_POOL(DescriptorTable, RZ_GFX_RESOURCE_TYPE_DESCRIPTOR_TABLE, 4096 * 64,   sizeof(rz_gfx_descriptor_table),   rzRHI_CreateDescriptorTable, rzRHI_DestroyDescriptorTable);
            // clang-format on
        }

        void RZResourceManager::ShutDown()
        {
            RAZIX_CORE_ERROR("[Resource Manager] Shutting Down Resource Manager");

            // Destroy all the Pools
            ////////////////////////////////
            RAZIX_UNREGISTER_RESOURCE_POOL(Texture);
            RAZIX_UNREGISTER_RESOURCE_POOL(Sampler);
            RAZIX_UNREGISTER_RESOURCE_POOL(Shader);
            RAZIX_UNREGISTER_RESOURCE_POOL(RootSignature);
            RAZIX_UNREGISTER_RESOURCE_POOL(Pipeline);
            RAZIX_UNREGISTER_RESOURCE_POOL(Buffer)
            RAZIX_UNREGISTER_RESOURCE_POOL(CommandPool);
            RAZIX_UNREGISTER_RESOURCE_POOL(CommandBuffer);
            RAZIX_UNREGISTER_RESOURCE_POOL(DescriptorHeap);
            RAZIX_UNREGISTER_RESOURCE_POOL(DescriptorTable);
            ////////////////////////////////

            RAZIX_UNREGISTER_RESOURCE_POOL(ResourceView);
        }

        //-----------------------------------------------------------------------------------

        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(ResourceView, RZ_GFX_RESOURCE_TYPE_RESOURCE_VIEW, rz_gfx_resource_view);

        //-----------------------------------------------------------------------------------

        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(Texture, RZ_GFX_RESOURCE_TYPE_TEXTURE, rz_gfx_texture);
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(Sampler, RZ_GFX_RESOURCE_TYPE_SAMPLER, rz_gfx_sampler);
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(Shader, RZ_GFX_RESOURCE_TYPE_SHADER, rz_gfx_shader);
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(RootSignature, RZ_GFX_RESOURCE_TYPE_ROOT_SIGNATURE, rz_gfx_root_signature);
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(Pipeline, RZ_GFX_RESOURCE_TYPE_PIPELINE, rz_gfx_pipeline);
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(Buffer, RZ_GFX_RESOURCE_TYPE_BUFFER, rz_gfx_buffer);
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(CommandPool, RZ_GFX_RESOURCE_TYPE_CMD_POOL, rz_gfx_cmdpool);
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(CommandBuffer, RZ_GFX_RESOURCE_TYPE_CMD_BUFFER, rz_gfx_cmdbuf);
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(DescriptorHeap, RZ_GFX_RESOURCE_TYPE_DESCRIPTOR_HEAP, rz_gfx_descriptor_heap);
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(DescriptorTable, RZ_GFX_RESOURCE_TYPE_DESCRIPTOR_TABLE, rz_gfx_descriptor_table);

        //-----------------------------------------------------------------------------------

    }    // namespace Gfx
}    // namespace Razix
