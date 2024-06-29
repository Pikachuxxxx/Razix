// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZResourceManager.h"

#include "Razix/Graphics/RHI/API/RZCommandPool.h"
#include "Razix/Graphics/RHI/API/RZDrawCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Utilities/RZStringUtilities.h"

#define CREATE_UTIL(type, pool)                                       \
    RZHandle<type> handle;                                            \
    void*          where = pool.obtain(handle);                       \
    type::Create(where, desc RZ_DEBUG_NAME_TAG_STR_E_ARG(desc.name)); \
    IRZResource<type>* resource = (IRZResource<type>*) where;         \
    resource->setName(desc.name);                                     \
    resource->setHandle(handle);                                      \
    return handle;

#define BEGIN_CREATE_UTIL(resourceName, ...)                                      \
    RZ##resourceName##Handle RZResourceManager::create##resourceName(__VA_ARGS__) \
    {                                                                             \
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);                       \
                                                                                  \
        RZHandle<RZ##resourceName> handle;                                        \
        void*                      where = m_##resourceName##Pool.obtain(handle);

#define END_CREATE_UTIL(resourceName)                                                 \
    IRZResource<RZ##resourceName>* resource = (IRZResource<RZ##resourceName>*) where; \
    resource->setHandle(handle);                                                      \
    return handle;                                                                    \
    }

#define END_CREATE_UTIL_NAMED(resourceName, poolName)                                 \
    IRZResource<RZ##resourceName>* resource = (IRZResource<RZ##resourceName>*) where; \
    resource->setName(poolName);                                                      \
    resource->setHandle(handle);                                                      \
    return handle;                                                                    \
    }

#define DESTROY_UTIL(pool, message) \
    if (handle.isValid())           \
        pool.release(handle);       \
    else                            \
        RAZIX_CORE_ERROR(message);

#define GET_UTIL(pool)           \
    if (handle.isValid())        \
        return pool.get(handle); \
    return nullptr;

#define RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(resourceName, arg)                                                                                                                        \
    RZ##resourceName##Handle RZResourceManager::create##resourceName(arg)                                                                                                            \
    {                                                                                                                                                                                \
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);                                                                                                                          \
        CREATE_UTIL(RZ##resourceName, m_##resourceName##Pool);                                                                                                                       \
    }                                                                                                                                                                                \
    void RZResourceManager::destroy##resourceName(RZ##resourceName##Handle handle)                                                                                                   \
    {                                                                                                                                                                                \
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);                                                                                                                          \
        DESTROY_UTIL(m_##resourceName##Pool, std::string("[Resource Manager] Attempting to release ") + std::string(#resourceName) + std::string(" resource with Invalid handle!")); \
    }                                                                                                                                                                                \
    RZ##resourceName* RZResourceManager::get##resourceName##Resource(RZ##resourceName##Handle handle)                                                                                \
    {                                                                                                                                                                                \
        GET_UTIL(m_##resourceName##Pool);                                                                                                                                            \
    }

#define RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS_DESTROY_GET(resourceName)                                                                                                                 \
    void RZResourceManager::destroy##resourceName(RZ##resourceName##Handle handle)                                                                                                   \
    {                                                                                                                                                                                \
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);                                                                                                                          \
        DESTROY_UTIL(m_##resourceName##Pool, std::string("[Resource Manager] Attempting to release ") + std::string(#resourceName) + std::string(" resource with Invalid handle!")); \
    }                                                                                                                                                                                \
    RZ##resourceName* RZResourceManager::get##resourceName##Resource(RZ##resourceName##Handle handle)                                                                                \
    {                                                                                                                                                                                \
        GET_UTIL(m_##resourceName##Pool);                                                                                                                                            \
    }

#define RAZIX_INIT_RESOURCE_POOL(resourceName, size) \
    m_##resourceName##Pool.init(size, RZ##resourceName::GetInstanceSize());

#ifdef RAZIX_DEBUG
    #define RAZIX_UNREGISTER_RESOURCE_POOL(resourceName) \
        m_##resourceName##Pool.printResources();         \
        m_##resourceName##Pool.destroy();
#else
    #define RAZIX_UNREGISTER_RESOURCE_POOL(resourceName) \
        m_##resourceName##Pool.destroy();
#endif

namespace Razix {
    namespace Graphics {

        void RZResourceManager::StartUp()
        {
            RAZIX_CORE_INFO("[Resource Manager] Starting Up Resource Manager");

            // Initialize all the Pools
            RAZIX_INIT_RESOURCE_POOL(Texture, 4096)
            RAZIX_INIT_RESOURCE_POOL(Shader, 512)
            RAZIX_INIT_RESOURCE_POOL(Pipeline, 512)
            RAZIX_INIT_RESOURCE_POOL(UniformBuffer, 4096)
            RAZIX_INIT_RESOURCE_POOL(CommandPool, 32)
            RAZIX_INIT_RESOURCE_POOL(DrawCommandBuffer, 32)
        }

        void RZResourceManager::ShutDown()
        {
            RAZIX_CORE_ERROR("[Resource Manager] Shutting Down Resource Manager");

            // Destroy all the Pools
            ////////////////////////////////
            RAZIX_UNREGISTER_RESOURCE_POOL(Texture)
            RAZIX_UNREGISTER_RESOURCE_POOL(Shader)
            RAZIX_UNREGISTER_RESOURCE_POOL(Pipeline)
            RAZIX_UNREGISTER_RESOURCE_POOL(UniformBuffer)
            RAZIX_UNREGISTER_RESOURCE_POOL(CommandPool)
            RAZIX_UNREGISTER_RESOURCE_POOL(DrawCommandBuffer)
            ////////////////////////////////
        }

        //-----------------------------------------------------------------------------------

        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(Texture, const RZTextureDesc& desc)
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(Shader, const RZShaderDesc& desc)
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(Pipeline, const RZPipelineDesc& desc)
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(UniformBuffer, const RZBufferDesc& desc)
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(VertexBuffer, const RZBufferDesc& desc)
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS(IndexBuffer, const RZBufferDesc& desc)

        // Since there don't use a Desc struct, we customize how they are created
        BEGIN_CREATE_UTIL(CommandPool, PoolType type)
        {
            RZCommandPool::Create(where, type);
        }
        END_CREATE_UTIL_NAMED(CommandPool, "CommandPool")
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS_DESTROY_GET(CommandPool)

        BEGIN_CREATE_UTIL(DrawCommandBuffer, RZCommandPoolHandle pool)
        {
            RZDrawCommandBuffer::Create(where, pool);
        }
        END_CREATE_UTIL_NAMED(DrawCommandBuffer, "DrawCommandBuffer")
        RAZIX_IMPLEMENT_RESOURCE_FUNCTIONS_DESTROY_GET(DrawCommandBuffer)

        //-----------------------------------------------------------------------------------

    }    // namespace Graphics
}    // namespace Razix
