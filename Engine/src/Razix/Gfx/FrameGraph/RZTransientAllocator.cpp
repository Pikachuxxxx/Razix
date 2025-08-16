// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZTransientAllocator.h"

#include "Razix/Gfx/FrameGraph/RZFrameGraph.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

namespace Razix {
    namespace Gfx {

        void AliasingEndTimeQueue::insert(u32 groupID, u32 end)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            AliasingPriorityEntry entry{groupID, end};
            m_Entries.emplace_back(entry);
            u32 i = m_Count;
            while (i > 0 && m_Entries[i - 1].end > end) {
                std::swap(m_Entries[i], m_Entries[i - 1]);
                --i;
            }
            ++m_Count;
        }

        void AliasingEndTimeQueue::update(u32 groupID, u32 newEnd)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            for (u32 i = 0; i < m_Count; ++i) {
                if (m_Entries[i].groupID == groupID) {
                    m_Entries[i].end = newEnd;
                    u32 j            = i;
                    while (j + 1 < m_Count && m_Entries[j].end > m_Entries[j + 1].end) {
                        std::swap(m_Entries[j], m_Entries[j + 1]);
                        ++j;
                    }
                    while (j > 0 && m_Entries[j - 1].end > m_Entries[j].end) {
                        std::swap(m_Entries[j - 1], m_Entries[j]);
                        --j;
                    }
                    break;
                }
            }
        }

        u32 AliasingEndTimeQueue::findFirstFree(u32 begin) const
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            for (u32 i = 0; i < m_Count; ++i) {
                if (m_Entries[i].end <= begin) {
                    return m_Entries[i].groupID;
                }
            }
            return UINT32_MAX;
        }

        void AliasingBook::build(std::vector<RZResourceLifetime> lifetimes)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            std::sort(lifetimes.begin(), lifetimes.end(), [](RZResourceLifetime& a, RZResourceLifetime& b) {
                return a.StartPassID < b.StartPassID;
            });
            m_Groups.reserve(64);
            m_Queue = AliasingEndTimeQueue();

            for (auto& lif: lifetimes) {
                uint32_t gid = m_Queue.findFirstFree(lif.StartPassID);
                if (gid != UINT32_MAX && m_Groups[gid].fits(lif)) {
                    m_Groups[gid].add(lif);
                    m_Queue.update(gid, lif.EndPassID);
                    m_ResourceToGroup[lif.ResourceEntryID] = gid;
                } else {
                    u32 newId = static_cast<u32>(m_Groups.size());
                    m_Groups.emplace_back(newId);
                    m_Groups.back().add(lif);
                    m_Queue.insert(newId, lif.EndPassID);
                    m_ResourceToGroup[lif.ResourceEntryID] = newId;
                }
            }
        }

#if 0

        void RZTransientAllocator::beginFrame()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // we create aliased resources based on the aliasing book and cache them
            m_AliasingBook.reset();
            m_AliasingBook.build(m_RegisteredLifetimes);

            //#if RAZIX_DEBUG
            //            auto& groups = m_AliasingBook.getGroups();
            //            for (u32 i = 0; i < groups.size(); i++) {
            //                const auto& group    = groups[i];
            //                const auto& entryIDs = group.getResourceEntryIDs();
            //
            //                if (entryIDs.size() >= 1) {
            //                    std::ostringstream oss;
            //                    oss << "+------------------------+\n";
            //                    oss << "| Aliasing Group " << i << "      |\n";
            //                    oss << "+------------------------+\n";
            //
            //                    for (u32 id: entryIDs) {
            //                        oss << "| Entry ID: " << std::setw(13) << std::left << id << "|\n";
            //                    }
            //
            //                    oss << "+------------------------+\n";
            //                    RAZIX_CORE_TRACE("\n{0}", oss.str());
            //                }
            //            }
            //#endif

            for (auto& group: m_AliasingBook.getGroups()) {
                // If the group has only one resource, we can create it directly
                if (group.getResourceEntriesSize() == 1) {
                    const auto&    entryIDs   = group.getResourceEntryIDs();
                    u32            resourceID = entryIDs[0];
                    FGResourceType resType    = m_FrameGraph.getResourceType(resourceID);

                    switch (resType) {
                        case FGResourceType::kFGTexture: {
                            auto& desc                 = m_FrameGraph.getResourceEntry(resourceID).getDescriptor<RZFrameGraphTexture>();
                            m_TextureCache[resourceID] = acquireTransientTexture(desc, resourceID);
                            break;
                        }
                        case FGResourceType::kFGBuffer: {
                            auto& desc                = m_FrameGraph.getResourceEntry(resourceID).getDescriptor<RZFrameGraphBuffer>();
                            m_BufferCache[resourceID] = acquireTransientBuffer(desc, resourceID);
                            break;
                        }
                        case FGResourceType::kFGSampler: {
                            auto& desc                 = m_FrameGraph.getResourceEntry(resourceID).getDescriptor<RZFrameGraphSampler>();
                            m_SamplerCache[resourceID] = acquireTransientSampler(desc, resourceID);
                            break;
                        }
                        default:
                            break;
                    }

                } else {
                    // TODO: use the Aliasing RHI API to create resources
                    // TOD: based on resource type generate a new combined T::Desc and use that to create the resource for the entire aliasing group
                    RAZIX_UNIMPLEMENTED_METHOD
                }
            }
        }

        void RZTransientAllocator::endFrame()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_RegisteredLifetimes.clear();
            // TODO: Clear any stale resources
        }

        void RZTransientAllocator::destroy()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Free all the cached resources
            for (auto& [key, handle]: m_TextureCache) {
                RZResourceManager::Get().destroyTexture(handle);
            }
            m_TextureCache.clear();

            for (auto& [key, handle]: m_BufferCache) {
                RZResourceManager::Get().destroyUniformBuffer(handle);
            }
            m_BufferCache.clear();

            for (auto& [key, handle]: m_SamplerCache) {
                RZResourceManager::Get().destroySampler(handle);
            }
            m_SamplerCache.clear();
        }

        rz_texture_handle RZTransientAllocator::acquireTransientTexture(const RZTextureDesc& desc, u32 id)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            // pop the handle from cache
            // if handle doesn't exist create a new one, return it, will be cached when we release it
            auto it = m_TextureCache.find(id);
            if (it != m_TextureCache.end()) {
                rz_texture_handle handle = it->second;
                m_TextureCache.erase(it);
                return handle;    // return the cached handle
            } else {
                // create a new texture
                rz_texture_handle handle = RZResourceManager::Get().createTexture(desc);
                return handle;
            }
        }

        void RZTransientAllocator::releaseTransientTexture(rz_texture_handle handle, u32 id)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_TextureCache[id] = handle;
        }

        RZUniformBufferHandle RZTransientAllocator::acquireTransientBuffer(const RZBufferDesc& desc, u32 id)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto it = m_BufferCache.find(id);
            if (it != m_BufferCache.end()) {
                RZUniformBufferHandle handle = it->second;
                m_BufferCache.erase(it);
                return handle;
            } else {
                return RZResourceManager::Get().createUniformBuffer(desc);
            }
        }

        void RZTransientAllocator::releaseTransientBuffer(RZUniformBufferHandle handle, u32 id)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            m_BufferCache[id] = handle;
        }

        RZSamplerHandle RZTransientAllocator::acquireTransientSampler(const RZSamplerDesc& desc, u32 id)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            auto it = m_SamplerCache.find(id);
            if (it != m_SamplerCache.end()) {
                RZSamplerHandle handle = it->second;
                m_SamplerCache.erase(it);
                return handle;
            } else {
                return RZResourceManager::Get().createSampler(desc);
            }
        }

        void RZTransientAllocator::releaseTransientSampler(RZSamplerHandle handle, u32 id)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
            m_SamplerCache[id] = handle;
        }
#endif
    }    // namespace Gfx
}    // namespace Razix
