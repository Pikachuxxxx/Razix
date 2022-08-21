// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZLight.h"

#include "Razix/Graphics/API/RZUniformBuffer.h"

// Constant scaling for objects when objects are moving
//const double fov                  = Math.PI / 4.0;    //suppose 45 degrees FOV
//float        cameraObjectDistance = Vector3.Distance(Camera.Position, Object.Position);
//float        worldSize            = (2 * Math.Tan(fov / 2.0)) * cameraObjectDistance;

namespace Razix {
    namespace Graphics {

        RZLight::RZLight(LightType type /*= LightType::DIRECTIONAL*/)
        {
            m_LightData.type = type;

            //switch (type) {
            //    case Razix::Graphics::RZLight::LightType::DIRECTIONAL:
            //        m_LightData     = &m_DirLightData;
            //        break;
            //    case Razix::Graphics::RZLight::LightType::POINT:
            //        m_LightData     = &m_PointLightData;
            //        break;
            //    case Razix::Graphics::RZLight::LightType::SPOT:
            //        m_LightData     = &m_SpotLightData;
            //    case Razix::Graphics::RZLight::LightType::AREA:
            //    case Razix::Graphics::RZLight::LightType::FLASH:
            //    default:
            //        m_LightData     = &m_DirLightData;
            //        break;
            //}
            //m_LightUBO = RZUniformBuffer::Create(m_LightDataSize, m_LightData, "Light Data");
        }

        RZLight::~RZLight()
        {
            //m_LightUBO->Destroy();
        }

        void RZLight::updateLight()
        {
            //m_LightUBO->SetData(m_LightDataSize, m_LightData);
        }

        void RZLight::setLightProperties(DirectionalLightData& dirLightData)
        {
            // Update the dir light and UBO
            m_LightData.dirLightData = dirLightData;
        }

        void RZLight::setLightProperties(PointLightData& pointLightData)
        {
            m_LightData.pointLightData = pointLightData;
        }

        void RZLight::setLightProperties(SpotLightData& sportLightData)
        {
            m_LightData.spotLightData = sportLightData;
        }

        void RZLight::setLightType(LightType type)
        {
            m_LightData.type = type;
            // TODO: Implement this and update the UBOs and data accordingly
        }

    }    // namespace Graphics
}    // namespace Razix