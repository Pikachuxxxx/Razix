#pragma once
#include "Razix/Gfx/Lighting/LightData.h"

#include <cereal/cereal.hpp>

namespace Razix {
    namespace Gfx {

        class RZUniformBuffer;

        class RAZIX_API RZLight
        {
        public:
            RZLight(LightType type = LightType::DIRECTIONAL);
            ~RZLight();

            // TODO: Add const to the get methods that are returning a reference

            RAZIX_INLINE LightType getType() { return m_LightData.type; }
            void                   setType(LightType type) { m_LightData.type = type; }

            RAZIX_INLINE LightData& getLightData() { return m_LightData; }
            void                    setLightType(LightType type);

            RAZIX_INLINE float3 getColor() { return m_LightData.color; }
            void                setColor(float3 color) { m_LightData.color = color; }

            RAZIX_INLINE float3 getDirection() { return float3(m_LightData.direction); }
            void                setDirection(const float3& direction) { m_LightData.direction = float4(direction, 1.0f); }

            RAZIX_INLINE f32 getRadius() { return m_LightData.range; }
            void             setRadius(f32 radius) { m_LightData.range = radius; }

            RAZIX_INLINE f32 getIntensity() { return m_LightData.intensity; }
            void             setIntensity(f32 value) { m_LightData.intensity = value; }

            RAZIX_INLINE const float3& getPosition() const { return m_LightData.position; }

            template<class Archive>
            void serialize(Archive& archive)
            {
                archive(cereal::make_nvp("type", m_LightData.type));
                archive(cereal::make_nvp("position", m_LightData.position));
                archive(cereal::make_nvp("range", m_LightData.range));
                archive(cereal::make_nvp("direction", m_LightData.direction));
                archive(cereal::make_nvp("color", m_LightData.color));
                archive(cereal::make_nvp("intensity", m_LightData.intensity));
                archive(cereal::make_nvp("constant", m_LightData.constantAttenuation));
                archive(cereal::make_nvp("linear", m_LightData.linearAttenuation));
                archive(cereal::make_nvp("quadratic", m_LightData.quadratic));
                archive(cereal::make_nvp("innerConeAngle", m_LightData.innerConeAngle));
                archive(cereal::make_nvp("outerConeAngle", m_LightData.outerConeAngle));
            }

        private:
            LightData m_LightData;
        };
    }    // namespace Gfx
}    // namespace Razix