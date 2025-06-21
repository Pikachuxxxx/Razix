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
            void reset();

        private:
            std::vector<AliasingPriorityEntry> m_Entries;
            sz                                 m_Count = 0;
        };

        class AliasingGroup
        {
        public:
            AliasingGroup(u32 id);
            bool                    fits(const RZResourceLifetime lifetime) const;
            void                    add(const RZResourceLifetime& lifetime);
            u32                     id() const;
            u32                     end() const;
            const std::vector<u32>& getResourceEntryIDs() const;

        private:
            u32              m_GroupID = UINT32_MAX;
            u32              m_MaxEnd  = 0;
            std::vector<u32> m_ResourceEntryIDs;
        };

        class AliasingBook
        {
        public:
            void                              build(std::vector<RZResourceLifetime> lifetimes);
            void                              reset();
            const std::vector<AliasingGroup>& getGroups() const;
            u32                               getGroupIDForResource(u32 resourceID) const;

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

            void registerLifetime(const RZResourceLifetime& lifetime);
            void bakeLifetimes();

            void destroy();

            RZTextureHandle acquireTransientTexture(const RZTextureDesc& desc);
            void            releaseTransientTexture(RZTextureHandle handle);

            RZUniformBufferHandle acquireTransientBuffer(const RZBufferDesc& desc);
            void                  releaseTransientBuffer(RZUniformBufferHandle handle);

            RZSamplerHandle acquireTransientSampler(const RZSamplerDesc& desc);
            void            releaseTransientSampler(RZSamplerHandle handle);

        private:
            AliasingBook                                                m_AliasingBook;
            std::vector<RZResourceLifetime>                             m_RegisteredLifetimes;
            std::unordered_map<u32, std::vector<RZTextureHandle>>       m_TextureCache;
            std::unordered_map<u32, std::vector<RZUniformBufferHandle>> m_BufferCache;
            std::unordered_map<u32, RZSamplerHandle>                    m_SamplerCache;
        };
    }    // namespace Gfx
}    // namespace Razix