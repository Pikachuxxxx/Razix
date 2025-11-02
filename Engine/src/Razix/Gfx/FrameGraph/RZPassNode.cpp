// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZPassNode.h"

#include "Razix/Core/std/utility.h"

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

namespace Razix {
    namespace Gfx {

        RAZIX_NO_DISCARD static bool hasId(const std::vector<RZFrameGraphResource>& v, RZFrameGraphResource id)
        {
#if __cpp_lib_ranges
            return std::ranges::find(v, id) != v.cend();
#else
            return std::find(v.cbegin(), v.cend(), id) != v.cend();
#endif
        }

        RAZIX_NO_DISCARD static bool hasId(const std::vector<RZFrameGraphResourceAcessView>& v, RZFrameGraphResource id)
        {
            const auto match = [id](const auto& e) { return e.id == id; };

#if __cpp_lib_ranges
            return std::ranges::find_if(v, match) != v.cend();
#else
            return std::find_if(v.cbegin(), v.cend(), match) != v.cend();
#endif
        }

        RAZIX_NO_DISCARD static bool hasId(const std::unordered_map<RZFrameGraphResource, RZFrameGraphResourceAcessView>& v, RZFrameGraphResource id)
        {
            return v.find(id) != v.end();
        }

        // TODO: Add a function check to check not just the ID but also the complete pair ID and flags
        // because when we want to mark a resource as read/write onto a pass it can have multiple access
        // views with different read/write properties, so check for the complete pair just in case
        // hasId is not as safe as contains but it's a lil faster so we only use it for existing resources

        //---------------------------------------------------------------------------

        RZPassNode::RZPassNode(const std::string_view name, u32 id, std::unique_ptr<IRZFrameGraphPass>&& exec)
            : RZGraphNode{name, id}, m_Exec{rz_move(exec)}
        {
            m_Creates.reserve(10);
            m_Reads.reserve(10);
            m_Writes.reserve(10);
        }

        bool RZPassNode::canCreateResouce(RZFrameGraphResource resourceID) const
        {
            return hasId(m_Creates, resourceID);
        }

        bool RZPassNode::canReadResouce(RZFrameGraphResource resourceID) const
        {
            return hasId(m_Reads, resourceID);
        }

        bool RZPassNode::canWriteResouce(RZFrameGraphResource resourceID) const
        {
            return hasId(m_Writes, resourceID);
        }

        void RZPassNode::createDeferredResourceView(RZFrameGraphResource id, rz_handle resHandle)
        {
            RAZIX_CORE_ASSERT(rz_handle_is_valid(&resHandle), "Invalid resource Handle passed!");

            auto& accessView = getResourceAccessViewRef(id);
            if (!rz_handle_is_valid(&accessView.resViewHandle)) {
                std::string resViewName;
                if (rzRHI_IsDescriptorTypeBuffer(accessView.resViewDesc.descriptorType)) {
                    if (accessView.resViewDesc.bufferViewDesc.pBuffer == RZ_FG_BUF_RES_AUTO_POPULATE) {
                        accessView.resViewDesc.bufferViewDesc.pBuffer = RZResourceManager::Get().getBufferResource(resHandle);
                        resViewName                                   = std::string(accessView.resViewDesc.bufferViewDesc.pBuffer->resource.pName);
                    } else {
                        RAZIX_CORE_ERROR("pBuffer is either NULL or not AUTO_POPULATE");
                    }
                } else if (rzRHI_IsDescriptorTypeTexture(accessView.resViewDesc.descriptorType)) {
                    if (accessView.resViewDesc.textureViewDesc.pTexture == RZ_FG_TEX_RES_AUTO_POPULATE) {
                        accessView.resViewDesc.textureViewDesc.pTexture = RZResourceManager::Get().getTextureResource(resHandle);
                        resViewName                                     = std::string(accessView.resViewDesc.textureViewDesc.pTexture->resource.pName);
                    } else {
                        RAZIX_CORE_ERROR("pTexture is not AUTO_POPULATE, this is invalid way to create resource views with framegraph, please follow the right convention by defining the pTexture with RZ_FG_TEX_RES_AUTO_POPULATE, to automatically generate and maintain resource views per pass");
                    }
                } else {
                    RAZIX_CORE_ERROR("unsupported resource view descriptor type in FrameGraph {0} for resource ID {1}", accessView.resViewDesc.descriptorType, id);
                    return;
                }

                // Now that we have filled the pResource create the resource view
                std::string resViewDebugName = std::string("ResView.") + resViewName + ".Pass." + getName();
                RAZIX_CORE_INFO("{}", resViewDebugName);
                accessView.resViewHandle = RZResourceManager::Get().createResourceView(resViewDebugName.c_str(), accessView.resViewDesc);
            }
        }

        bool RZPassNode::canExecute() const
        {
            return m_RefCount > 0 || isStandAlone();
        }

        RZFrameGraphResource RZPassNode::registerResourceForRead(RZFrameGraphResource id, rz_gfx_resource_view_desc viewDesc)
        {
            RAZIX_CORE_ASSERT((!canCreateResouce(id) && !canWriteResouce(id)), "Cannot read a resource that this node creates or write to!");
            if (canReadResouce(id))
                return id;

            m_Reads.emplace(id, RZFrameGraphResourceAcessView{id, viewDesc});
            return id;
        }

        RZFrameGraphResource RZPassNode::registerResourceForWrite(RZFrameGraphResource id, rz_gfx_resource_view_desc viewDesc)
        {
            if (canWriteResouce(id))
                return id;

            m_Writes.emplace(id, RZFrameGraphResourceAcessView{id, viewDesc});
            return id;
        }

        void RZPassNode::destroyDeferredResourceViews()
        {
            for (auto& [id, accessView]: m_Reads) {
                if (rz_handle_is_valid(&accessView.resViewHandle)) {
                    RZResourceManager::Get().destroyResourceView(accessView.resViewHandle);
                    accessView.resViewHandle = {};
                }
            }
            for (auto& [id, accessView]: m_Writes) {
                if (rz_handle_is_valid(&accessView.resViewHandle)) {
                    RZResourceManager::Get().destroyResourceView(accessView.resViewHandle);
                    accessView.resViewHandle = {};
                }
            }
        }

    }    // namespace Gfx
}    // namespace Razix
