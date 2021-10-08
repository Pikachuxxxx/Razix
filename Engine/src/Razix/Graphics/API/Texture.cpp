#include "rzxpch.h"
#include "Texture.h"


namespace Razix {
    namespace Graphics {

        void Texture2D::Bind(uint32_t slot /*= 0*/) const {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void Texture2D::Unbind(uint32_t /*= 0*/) const {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void Texture2D::SetData(const void* pixels) {
            throw std::logic_error("The method or operation is not implemented.");
        }

    }
}