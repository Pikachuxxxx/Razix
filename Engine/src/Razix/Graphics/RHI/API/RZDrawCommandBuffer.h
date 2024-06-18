#pragma once

#include "Razix/Core/RZDebugConfig.h"

namespace Razix {
    namespace Graphics {

        class RZCommandAllocatorPool;

        enum class CommandBufferState : u8
        {
            Idle,
            Recording,
            Ended,
            Submitted
        };

        /**
         * Command buffer to which the draw and other command are recorded to and used with
         * Note: Command Buffers needs command pool to be allocated from, except for single time 
         * command buffers. The Device can store the command allocators in ring buffer and uses that 
         * to allocate the command buffers to a ring buffer as the needed for multiple and
         * multi-threaded recording usage etc. in future.
         */
        class RAZIX_API RZDrawCommandBuffer : public RZRoot
        {
        public:
            RZDrawCommandBuffer()          = default;
            virtual ~RZDrawCommandBuffer() = default;

            static RZDrawCommandBuffer* BeginSingleTimeCommandBuffer();

            static void EndSingleTimeCommandBuffer(RZDrawCommandBuffer* cmdBuffer);

            /* Initializes the command buffer and creates them */
            virtual void Init(RZ_DEBUG_NAME_TAG_S_ARG) = 0;
            /* Starts the recording of the commands onto the command buffer */
            virtual void BeginRecording() = 0;
            /* Ends the command buffer recording */
            virtual void EndRecording() = 0;
            /* Executes the command buffer by submitting to the command queue */
            virtual void Execute() = 0;
            /* Resets the command buffer of all the recordings */
            virtual void Reset() = 0;

            virtual void*      getAPIBuffer() { return nullptr; }
            CommandBufferState getState() const { return m_State; }

        protected:
            CommandBufferState m_State = CommandBufferState::Idle;

        private:
            /**
             * Creates a command buffer to record draw command onto and bind pipeline and it's resources while drawing
             * 
             * @returns Returns the handle to the abstracted underlying graphics API implementation of the RZCommandBuffer
             */
            static RZDrawCommandBuffer* Create(RZCommandAllocatorPool* pool);

            friend class RZResourceManager;
        };

    }    // namespace Graphics
}    // namespace Razix