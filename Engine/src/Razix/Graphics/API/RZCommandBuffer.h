#pragma once

namespace Razix {
    namespace Graphics {

        /* Command buffer to which the draw and other command are recorded to and used with */
        class RAZIX_API RZCommandBuffer
        {
        public:
            RZCommandBuffer() = default;
            virtual ~RZCommandBuffer() = default;

            /**
             * Creates a command buffer to record draw command onto and bind pipeline and it's resources while drawing
             * 
             * @returns Returns the handle to the abstracted underlying graphics API implementation of the RZCommandBuffer
             */
            static RZCommandBuffer* Create();

            /* Initializes the command buffer and creates them */
            virtual void Init() = 0;
            /* Starts the recording of the commands onto the command buffer */
            virtual void BeginRecording() = 0;
            /* Ends the command buffer recording */
            virtual void EndRecording() = 0;
            /* Executes the command buffer by submitting to the command queue */
            virtual void Execute() = 0;
            /* Resets the command buffer of all the recordings */
            virtual void Reset() = 0;

            /*
            // Draw command for the command buffer
            /* Draw the vertex buffer vertex data */
            //virtual void Draw(uint32_t verticexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
            /* Draw the vertex data from the vertex buffer using the indices from the index data 
            virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) = 0;
            */

            /* Updates the viewport dynamically with the given width and height */
            virtual void UpdateViewport(uint32_t width, uint32_t height) = 0;

            virtual void* getAPIBuffer() { return nullptr; }
        };

    }
}