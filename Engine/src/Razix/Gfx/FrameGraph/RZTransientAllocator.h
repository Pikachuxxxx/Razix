#pragma once

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

namespace Razix {
    namespace Gfx {

        class RZFrameGraph;

        struct RAZIX_API AliasingPriorityEntry
        {
            u32 groupID;
            u32 end;
        };

        class RAZIX_API AliasingEndTimeQueue
        {
        public:
            void insert(u32 groupID, u32 end);
            void update(u32 groupID, u32 newEnd);
            u32  findFirstFree(u32 begin) const;

            inline void reset()
            {
                m_Entries.clear();
                m_Count = 0;
            }

        private:
            std::vector<AliasingPriorityEntry> m_Entries;
            u32                                m_Count = 0;
        };

        class RAZIX_API AliasingGroup
        {
        public:
            AliasingGroup(u32 id)
                : m_GroupID(id), m_MaxEnd(0) {}

            inline bool fits(const RZResourceLifetime lifetime) const
            {
                return lifetime.StartPassID > m_MaxEnd;
            }

            inline void add(const RZResourceLifetime& lifetime)
            {
                m_ResourceEntryIDs.push_back(lifetime.ResourceEntryID);
                m_MaxEnd = std::max(m_MaxEnd, lifetime.EndPassID);
            }
            inline u32                     id() const { return m_GroupID; }
            inline u32                     end() const { return m_MaxEnd; }
            inline const std::vector<u32>& getResourceEntryIDs() const { return m_ResourceEntryIDs; }
            inline u32                     getResourceEntriesSize() const { return static_cast<u32>(m_ResourceEntryIDs.size()); }

        private:
            u32              m_GroupID = UINT32_MAX;
            u32              m_MaxEnd  = 0;
            std::vector<u32> m_ResourceEntryIDs;
        };

        // TODO: Sort groups by resource types! no mix and match allowed
        class RAZIX_API AliasingBook
        {
        public:
            void build(std::vector<RZResourceLifetime> lifetimes);

            inline void reset()
            {
                m_Groups.clear();
                m_Queue.reset();
            }
            inline const std::vector<AliasingGroup>& getGroups() const { return m_Groups; }
            inline u32                               getGroupIDForResource(u32 resourceID) const
            {
                auto it = m_ResourceToGroup.find(resourceID);
                if (it != m_ResourceToGroup.end()) {
                    return it->first;
                } else
                    return UINT32_MAX;
            }

        private:
            std::vector<AliasingGroup>   m_Groups;
            AliasingEndTimeQueue         m_Queue;
            std::unordered_map<u32, u32> m_ResourceToGroup;
        };

        class RAZIX_API RZTransientAllocator
        {
        public:
            RZTransientAllocator(const RZFrameGraph& fg)
                : m_FrameGraph(fg) {}
            // TODO: This uses the compiled resource entries and their descriptions to build aliased resources for the frame
            // TODO: For now it uses their default RHI API but once we have aliasable resources, we can use the extended RHI API to alias
            void beginFrame() {}
            void endFrame() {}

            // Frees memory for all resources held by this allocator from ResourceManager
            void destroy() {}

            //rz_texture_handle       acquireTransientTexture(const RZTextureDesc& desc, u32 id);
            //void                  releaseTransientTexture(rz_texture_handle handle, u32 id);
            //RZUniformBufferHandle acquireTransientBuffer(const RZBufferDesc& desc, u32 id);
            //void                  releaseTransientBuffer(RZUniformBufferHandle handle, u32 id);
            //RZSamplerHandle       acquireTransientSampler(const RZSamplerDesc& desc, u32 id);
            //void                  releaseTransientSampler(RZSamplerHandle handle, u32 id);

            inline void                registerLifetime(const RZResourceLifetime& lifetime) { m_RegisteredLifetimes.push_back(lifetime); }
            inline const AliasingBook& getAliasBook() const { return m_AliasingBook; }

        private:
            AliasingBook                    m_AliasingBook;
            std::vector<RZResourceLifetime> m_RegisteredLifetimes;
            //std::unordered_map<u32, rz_texture_handle>       m_TextureCache;
            //std::unordered_map<u32, RZUniformBufferHandle> m_BufferCache;
            //std::unordered_map<u32, RZSamplerHandle>       m_SamplerCache;
            const Gfx::RZFrameGraph& m_FrameGraph;
        };

#define TRANSIENT_ALLOCATOR_CAST(x) ((RZTransientAllocator*) x)

    }    // namespace Gfx
}    // namespace Razix