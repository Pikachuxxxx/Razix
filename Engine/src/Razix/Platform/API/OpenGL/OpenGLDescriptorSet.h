#pragma once

#include "Razix/Graphics/API/RZDescriptorSet.h"

namespace Razix {
    namespace Graphics {

        class OpenGLDescriptorSet : public RZDescriptorSet
        {
        public:
            OpenGLDescriptorSet(const std::vector<RZDescriptor>& descriptors);
            ~OpenGLDescriptorSet();

            void UpdateSet(const std::vector<RZDescriptor>& descriptors) override;
            void Destroy() override;

            RAZIX_INLINE std::vector<RZDescriptor> getDescriptors() { return m_Descriptors; }

        private:
            std::vector<RZDescriptor> m_Descriptors;
        };
    }    // namespace Graphics
}    // namespace Razix