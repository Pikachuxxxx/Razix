#pragma once

#include "Razix/Gfx/Resources/IRZResource.h"

namespace Razix {
    namespace Gfx {

        enum class PoolType : u32
        {
            kGraphics,
            kCompute,
            kCopy
        };

        class RZCommandPool : public IRZResource<RZCommandPool>
        {
        public:
            RZCommandPool()          = default;
            virtual ~RZCommandPool() = default;

            GET_INSTANCE_SIZE;

            /* Resets the command buffers recordings and the resources it holds */
            virtual void  Reset()        = 0;
            virtual void* getAPIHandle() = 0;

            RZDrawCommandBufferHandle allocateCommandBuffer();
            RAZIX_INLINE PoolType     getPoolType() const { return m_PoolType; }

        protected:
            PoolType m_PoolType = PoolType::kGraphics; /* Type of command buffers allocated from this pool */

        private:
            /**
             * Creates a command pool to allocated command buffers from
             * 
             * @param type The type of the pool to tell what kind of command buffers are allocated from the pool
             */
            static void Create(void* where, PoolType type);

            friend class RZResourceManager;
        };

    }    // namespace Graphics
}    // namespace Razix
