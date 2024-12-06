// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZLight.h"

namespace Razix {
    namespace Graphics {

        RZLight::RZLight(LightType type /*= LightType::DIRECTIONAL*/)
        {
            m_LightData.type = type;
        }

        RZLight::~RZLight()
        {
        }

        void RZLight::setLightType(LightType type)
        {
            m_LightData.type = type;
        }
         

    }    // namespace Graphics
}    // namespace Razix