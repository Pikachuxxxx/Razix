// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZAssetBase.h"

#include "Razix/Core/Memory/RZMemoryFunctions.h"

namespace Razix {

    RZAsset::RZAsset(RZAssetType type, void* pColdDataMemory)
    {
        memset(&m_Hot, 0, sizeof(m_Hot));
        m_Hot.UUID = RZUUID();
        m_Hot.type = type;

        if (m_pCold == NULL || pColdDataMemory == NULL) {
            m_pCold = (RZAssetColdData*) rz_malloc_aligned(sizeof(RZAssetColdData));
        } else {
            m_pCold = (RZAssetColdData*) pColdDataMemory;
        }
        memset(m_pCold, 0, sizeof(RZAssetColdData));
        rz_critical_section_destroy(&m_pCold->CS);
    }

    void RZAsset::destroy()
    {
        if (m_pCold) {
            rz_critical_section_destroy(&m_pCold->CS);
            // Don't free cold data here as its managed by the asset pool
            // You do not own the memory! and we don't want double frees.
            // The freelist in the asset pool will take care of freeing the cold data memory
            //rz_free(m_pCold);
            m_pCold = NULL;
        }
    }
}    // namespace Razix
