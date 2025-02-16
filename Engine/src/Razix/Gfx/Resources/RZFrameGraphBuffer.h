#pragma once



namespace Razix {
    namespace Gfx {
        namespace FrameGraph {

            class RZFrameGraphBuffer
            {
            public:
                typedef RZBufferDesc Desc;

                void create(const Desc& desc, void* transientAllocator);
                void destroy(const Desc& desc, void* transientAllocator);

                static std::string toString(const Desc& desc);

                void preRead(const Desc& desc, uint32_t flags);
                void preWrite(const Desc& desc, uint32_t flags);

                Gfx::RZUniformBufferHandle getHandle() { return m_BufferHandle; }

            public:
                Gfx::RZUniformBufferHandle m_BufferHandle;
            };
        }    // namespace FrameGraph
    }    // namespace Gfx
}    // namespace Razix