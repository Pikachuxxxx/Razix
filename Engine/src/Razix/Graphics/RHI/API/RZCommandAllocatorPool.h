#pragma once

namespace Razix {
    namespace Graphics {

        enum class PoolType
        {
            kGraphics,
            kCompute,
            kCopy
        };

        class RZCommandAllocatorPool
        {
        public:
            RZCommandAllocatorPool()          = default;
            virtual ~RZCommandAllocatorPool() = default;

            /* Resets the command buffers of all the recordings */
            virtual RZDrawCommandBufferHandle AllocateCommandBuffer() = 0;
            /* Resets the command buffers and the resources it holds */
            virtual void Reset() = 0;

            virtual void* getAPIHandle() { return nullptr; }
            PoolType      getPoolType() const { return m_PoolType; }

        protected:
            PoolType                                m_PoolType   = PoolType::kGraphics; /* Type of command buffers allocated from this pool */
            u32                                     m_FenceValue = 0;                   /* For in-flight frames synchronization */
            rzstl::ring_buffer<RZDrawCommandBuffer> m_DrawCommandBuffers;
            rzstl::ring_buffer<RZDrawCommandBuffer> m_CopyCommandBuffers;
            //rzstl::ring_buffer<RZDrawCommandBuffer> m_ComputeCommandBuffers;
            //rzstl::ring_buffer<RZDrawCommandBuffer> m_AsyncComputeCommandBuffers;

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
