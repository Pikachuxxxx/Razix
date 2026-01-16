// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZAssetBase.h"

#include "Razix/Core/Memory/RZMemoryFunctions.h"
#include <Core/RZCore.h>

namespace Razix {

    RZAsset::RZAsset(RZAssetType type, void* pColdDataMemory)
    {
        RAZIX_CORE_ASSERT(pColdDataMemory != NULL, "[Asset] pColdDataMemory is NULL, creating asset with provided memory.");

        memset(&m_Hot, 0, sizeof(m_Hot));
        m_Hot.UUID = RZUUID();
        m_Hot.type = type;

        m_pCold = (RZAssetColdData*) pColdDataMemory;

        memset(m_pCold, 0, sizeof(RZAssetColdData));
        m_pCold->CS = rz_critical_section_create();
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
