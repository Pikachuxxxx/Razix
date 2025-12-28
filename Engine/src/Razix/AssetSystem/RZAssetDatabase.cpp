// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZAssetDatabase.h"

namespace Razix {
    namespace AssetSystem {
    }

    void RZAssetDatabase::init()
    {
        // Initialize the header pool with a default capacity
        // TODO: Make this configurable
        m_HeaderPool.init(4096);
    }

    void RZAssetDatabase::shutDown()
    {
        // Destroy all registered pools
        for (u32 i = 0; i < (u32)RZAssetType::COUNT; ++i)
        {
            if (m_AssetPools[i])
            {
                RZAssetPoolBase* pool = static_cast<RZAssetPoolBase*>(m_AssetPools[i]);
                pool->destroy();
                delete pool;
                m_AssetPools[i] = nullptr;
            }
        }

        m_HeaderPool.destroy();
    }

}    // namespace Razix