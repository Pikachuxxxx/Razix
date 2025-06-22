#pragma once

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

namespace Razix {
    namespace Gfx {

        struct AliasingPriorityEntry
        {
            u32 groupID;
            u32 end;
        };

        class AliasingEndTimeQueue
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

        class AliasingGroup
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

        private:
            u32              m_GroupID = UINT32_MAX;
            u32              m_MaxEnd  = 0;
            std::vector<u32> m_ResourceEntryIDs;
        };

        class AliasingBook
        {
        public:
            void build(std::vector<RZResourceLifetime> lifetimes);

            inline void reset()
            {
                m_Groups.clear();
                m_Queue.reset();
            }
            inline const std::vector<AliasingGroup>& getGroups() const { return m_Groups; }
            inline u32                               getGroupIDForResource(u32 resourceID) const { return m_ResourceToGroup.at(resourceID); }

        private:
            std::vector<AliasingGroup>   m_Groups;
            AliasingEndTimeQueue         m_Queue;
            std::unordered_map<u32, u32> m_ResourceToGroup;
        };

        class RZTransientAllocator
        {
        public:
            void beginFrame();
            void endFrame();

            void registerLifetime(const RZResourceLifetime& lifetime)
            {
                m_RegisteredLifetimes.push_back(lifetime);
            }
            void bakeLifetimes();

            void destroy();

            RZTextureHandle acquireTransientTexture(const RZTextureDesc& desc);
            void            releaseTransientTexture(RZTextureHandle handle);

            RZUniformBufferHandle acquireTransientBuffer(const RZBufferDesc& desc);
            void                  releaseTransientBuffer(RZUniformBufferHandle handle);

            RZSamplerHandle acquireTransientSampler(const RZSamplerDesc& desc);
            void            releaseTransientSampler(RZSamplerHandle handle);
            
            const AliasingBook& getAliasBook() const { return m_AliasingBook; }

        private:
            AliasingBook                                                m_AliasingBook;
            std::vector<RZResourceLifetime>                             m_RegisteredLifetimes;
            std::unordered_map<u32, std::vector<RZTextureHandle>>       m_TextureCache;
            std::unordered_map<u32, std::vector<RZUniformBufferHandle>> m_BufferCache;
            std::unordered_map<u32, RZSamplerHandle>                    m_SamplerCache;
        };
    }    // namespace Gfx
}    // namespace Razix
