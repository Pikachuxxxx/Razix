#pragma once

#include "Razix/Core/RZDebugConfig.h"

namespace Razix {
    namespace Graphics {

        enum class CommandBufferState : uint8_t
        {
            Idle,
            Recording,
            Ended,
            Submitted
        };

        /* Command buffer to which the draw and other command are recorded to and used with */
        class RAZIX_API RZCommandBuffer : public RZRoot
        {
        public:
            RZCommandBuffer()          = default;
            virtual ~RZCommandBuffer() = default;

            /**
             * Creates a command buffer to record draw command onto and bind pipeline and it's resources while drawing
             * 
             * @returns Returns the handle to the abstracted underlying graphics API implementation of the RZCommandBuffer
             */
            static RZCommandBuffer* Create();

            /* Initializes the command buffer and creates them */
            virtual void Init(NAME_TAG_F) = 0;
            /* Starts the recording of the commands onto the command buffer */
            virtual void BeginRecording() = 0;
            /* Ends the command buffer recording */
            virtual void EndRecording() = 0;
            /* Executes the command buffer by submitting to the command queue */
            virtual void Execute() = 0;
            /* Resets the command buffer of all the recordings */
            virtual void Reset() = 0;

            /* Updates the viewport dynamically with the given width and height */
            virtual void UpdateViewport(uint32_t width, uint32_t height) = 0;

            virtual void*      getAPIBuffer() { return nullptr; }
            CommandBufferState getState() { return m_State; }

        protected:
            CommandBufferState m_State;
        };

    }    // namespace Graphics
}    // namespace Razix