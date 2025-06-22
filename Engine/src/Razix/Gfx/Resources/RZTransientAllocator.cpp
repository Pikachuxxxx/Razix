// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZTransientAllocator.h"

namespace Razix {
    namespace Gfx {
    
    /*
     m_TestAliasbook.build(flatResEntries);

     auto& groups = m_TestAliasbook.getGroups();
     for (u32 i = 0; i < groups.size(); i++) {
         const auto& group    = groups[i];
         const auto& entryIDs = group.getResourceEntryIDs();

         if (entryIDs.size() >= 1) {
             std::ostringstream oss;
             oss << "+------------------------+\n";
             oss << "| Aliasing Group " << i << "      |\n";
             oss << "+------------------------+\n";

             for (u32 id: entryIDs) {
                 oss << "| Entry ID: " << std::setw(13) << std::left << id << "|\n";
             }

             oss << "+------------------------+\n";
             RAZIX_CORE_TRACE("\n{0}", oss.str());
         }
     }
     */

        void AliasingEndTimeQueue::insert(u32 groupID, u32 end)
        {
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
            for (u32 i = 0; i < m_Count; ++i) {
                if (m_Entries[i].end <= begin) {
                    return m_Entries[i].end;
                }
            }
            return UINT32_MAX;
        }

        void AliasingBook::build(std::vector<RZResourceLifetime> lifetimes)
        {
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

    }    // namespace Gfx
}    // namespace Razix
