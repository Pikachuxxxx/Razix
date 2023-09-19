// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZResourceManager.h"

#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"

#include "Razix/Utilities/RZStringUtilities.h"

namespace Razix {
    namespace Graphics {

        void RZResourceManager::StartUp()
        {
            RAZIX_CORE_INFO("[Resource Manager] Starting Up Resource Manager");

            // TODO: Use static methods on resource to calculate the actual size of the underlying API class implementations
            // Initialize all the Pools
            m_TexturePool.init(512, RZTexture::GetInstanceSize());
            m_ShaderPool.init(128, RZShader::GetInstanceSize());
            m_PipelinePool.init(128, RZTexture::GetInstanceSize());
            //m_VertexBufferPool.init(128);
            //m_IndexBufferPool.init(128);
        }

        void RZResourceManager::ShutDown()
        {
            RAZIX_CORE_ERROR("[Resource Manager] Shutting Down Resource Manager");

            // Destroy all the Pools
            ////////////////////////////////
            m_TexturePool.printResources();
            m_TexturePool.destroy();
            ////////////////////////////////
            m_ShaderPool.printResources();
            m_ShaderPool.destroy();
            ////////////////////////////////
            m_PipelinePool.printResources();
            m_PipelinePool.destroy();
            ////////////////////////////////
            //m_VertexBufferPool.destroy();
            //m_IndexBufferPool.destroy();
        }

        //-----------------------------------------------------------------------------------

        RZTextureHandle RZResourceManager::createTexture(const RZTextureDesc& desc)
        {
            // Use the Pool
            RZHandle<RZTexture> handle;
            void*               where = m_TexturePool.obtain(handle);
            RZTexture::Create(where, desc RZ_DEBUG_NAME_TAG_STR_E_ARG(desc.name));
            IRZResource<RZTexture>* resource = (IRZResource<RZTexture>*) where;
            resource->setName(desc.name);
            resource->setHandle(handle);
            //((RZTexture*) where)->UploadToBindlessSet();
            return handle;
        }

        RZTextureHandle RZResourceManager::createTextureFromFile(const RZTextureDesc& desc, const std::string& filePath)
        {
            RZHandle<RZTexture> handle;
            void*               where = m_TexturePool.obtain(handle);
            RZTexture::CreateFromFile(where, desc, filePath RZ_DEBUG_NAME_TAG_STR_E_ARG(desc.name));
            IRZResource<RZTexture>* resource = (IRZResource<RZTexture>*) where;
            resource->setName(desc.name);
            resource->setHandle(handle);
#ifdef ENABLE_BINDLESS
            ((RZTexture*) where)->UploadToBindlessSet();
#endif
            return handle;
        }

        void RZResourceManager::destroyTexture(RZTextureHandle handle)
        {
            // Delete only if it's a valid handle, else skip it and report it
            if (handle.isValid())
                m_TexturePool.release(handle);
            else
                RAZIX_CORE_ERROR("[Resource Manager] Attempting to release a Texture resource with Invalid handle!");
        }

        RZTexture* RZResourceManager::getTextureResource(RZTextureHandle handle)
        {
            return m_TexturePool.get(handle);
        }

        //-----------------------------------------------------------------------------------

        Razix::Graphics::RZShaderHandle RZResourceManager::createShaderFromFile(ShaderBuiltin shaderID, std::string shaderPath)
        {
            RZHandle<RZShader> handle;
            void*              where = m_ShaderPool.obtain(handle);
            RZShader::Create(where, shaderPath RZ_DEBUG_NAME_TAG_STR_E_ARG(shaderPath));
            ((RZShader*) where)->m_ShaderLibraryID = shaderID;
            IRZResource<RZShader>* resource        = (IRZResource<RZShader>*) where;
            resource->setName(Utilities::GetFileName(shaderPath));
            resource->setHandle(handle);
            return handle;
        }

        void RZResourceManager::destroyShader(RZShaderHandle handle)
        {
            // Delete only if it's a valid handle, else skip it and report it
            if (handle.isValid())
                m_ShaderPool.release(handle);
            else
                RAZIX_CORE_ERROR("[Resource Manager] Attempting to release a Shader resource with Invalid handle!");
        }

        RZShader* RZResourceManager::getShaderResource(RZShaderHandle handle)
        {
            return m_ShaderPool.get(handle);
        }

        //-----------------------------------------------------------------------------------

        Razix::Graphics::RZPipelineHandle RZResourceManager::createPipeline(const RZPipelineDesc& desc)
        {
            RZHandle<RZPipeline> handle;
            void*                where = m_PipelinePool.obtain(handle);
            RZPipeline::Create(where, desc RZ_DEBUG_NAME_TAG_STR_E_ARG(desc.name));
            IRZResource<RZPipeline>* resource = (IRZResource<RZPipeline>*) where;
            resource->setName(Utilities::GetFileName(desc.name));
            resource->setHandle(handle);
            return handle;
        }

        void RZResourceManager::destroyPipeline(RZPipelineHandle handle)
        {
            // Delete only if it's a valid handle, else skip it and report it
            if (handle.isValid())
                m_PipelinePool.release(handle);
            else
                RAZIX_CORE_ERROR("[Resource Manager] Attempting to release a Pipeline resource with Invalid handle!");
        }

        RZPipeline* RZResourceManager::getPipelineResource(RZPipelineHandle handle)
        {
            return m_PipelinePool.get(handle);
        }

        //-----------------------------------------------------------------------------------

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
