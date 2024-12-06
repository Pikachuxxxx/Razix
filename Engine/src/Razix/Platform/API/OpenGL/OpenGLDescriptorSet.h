#pragma once

#include "Razix/Graphics/RHI/API/RZDescriptorSet.h"

namespace Razix {
    namespace Graphics {

        class OpenGLDescriptorSet : public RZDescriptorSet
        {
        public:
            OpenGLDescriptorSet(const std::vector<RZDescriptor>& descriptors);
            ~OpenGLDescriptorSet();

            void UpdateSet(const std::vector<RZDescriptor>& descriptors, bool layoutTransition = true) override;
            void Destroy() const override;

            RAZIX_INLINE const std::vector<RZDescriptor>& getDescriptors() const { return m_Descriptors; }

        private:
            std::vector<RZDescriptor> m_Descriptors;
        };
    }    // namespace Graphics
}    // namespace Razix