// clang-format off
#include "rzxpch.h"
// clang-format on
#include "OpenGLDescriptorSet.h"

namespace Razix {
    namespace Graphics {

        OpenGLDescriptorSet::OpenGLDescriptorSet(const std::vector<RZDescriptor>& descriptors)
            : m_Descriptors(descriptors)
        {
        }

        OpenGLDescriptorSet::~OpenGLDescriptorSet()
        {
        }

        void OpenGLDescriptorSet::UpdateSet(const std::vector<RZDescriptor>& descriptors, bool layoutTransition)
        {
            // Update the uniform buffers here (don't we already get updated ones?)
        }

        void OpenGLDescriptorSet::Destroy() const
        {
        }

    }    // namespace Graphics
}    // namespace Razix