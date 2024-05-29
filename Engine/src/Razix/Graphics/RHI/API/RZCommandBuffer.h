#pragma once

#include "Razix/Core/RZDebugConfig.h"

namespace Razix {
    namespace Graphics {

        enum class CommandBufferState : u8
        {
            Idle,
            Recording,
            Ended,
            Submitted
        };

        // FIXME: Allocate command buffer from separate command pool per back buffer framws

        /* Command buffer to which the draw and other command are recorded to and used with */
        class RAZIX_API RZCommandBuffer : public RZRoot
        {
        public:
            RZCommandBuffer()          = default;
            virtual ~RZCommandBuffer() = default;

            static RZCommandBuffer* BeginSingleTimeCommandBuffer();

            static void EndSingleTimeCommandBuffer(RZCommandBuffer* cmdBuffer);

            /**
             * Creates a command buffer to record draw command onto and bind pipeline and it's resources while drawing
             * 
             * @returns Returns the handle to the abstracted underlying graphics API implementation of the RZCommandBuffer
             */
            static RZCommandBuffer* Create();

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
        };

    }    // namespace Graphics
}    // namespace Razix