#pragma once

#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphSemaphore.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphTexture.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            class RAZIX_API RZTransientResources
            {
            public:
                RZTransientResources()                                 = delete;
                RZTransientResources(const RZTransientResources &)     = delete;
                RZTransientResources(RZTransientResources &&) noexcept = delete;
                ~RZTransientResources() {}

                RZTransientResources &operator=(const RZTransientResources &)     = delete;
                RZTransientResources &operator=(RZTransientResources &&) noexcept = delete;

                void destroyResources();

                RZTexture *acquireTexture(const RZFrameGraphTexture::Desc &desc);
                void       releaseTexture(const RZFrameGraphTexture::Desc &desc, RZTexture *texture);

                RZSemaphore *acquireSemaphore(const RZFrameGraphSemaphore::Desc &desc);
                void         releaseSemaphore(const RZFrameGraphSemaphore::Desc &desc, RZSemaphore *semaphore);

            private:
                std::vector<std::unique_ptr<RZTexture*>>   m_Textures;
                std::vector<std::unique_ptr<RZSemaphore*>> m_Semaphores;

                template<typename T>
                struct ResourceEntry
                {
                    T     resource;
                    float life;
                };
                template<typename T>
                using ResourcePool = std::vector<ResourceEntry<T>>;

                std::unordered_map<std::size_t, ResourcePool<RZTexture *>>   m_TexturePools;
                std::unordered_map<std::size_t, ResourcePool<RZSemaphore *>> m_SemaphorePools;
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
