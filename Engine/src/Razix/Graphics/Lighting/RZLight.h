#pragma once
#include "Razix/Graphics/Lighting/LightData.h"

namespace Razix {
    namespace Graphics {

        class RZUniformBuffer;
#if 0
        struct DirectionalLightData
        {
            alignas(16) glm::vec3 direction = glm::vec3(0.0f);
            alignas(16) glm::vec3 color     = glm::vec3(0.0f);
            alignas(16) f32 time          = 0.0f;

            template<class Archive>
            void serialize(Archive& archive)
            {
                archive(cereal::make_nvp("direction", direction));
                archive(cereal::make_nvp("color", color));
                archive(cereal::make_nvp("time", time));
            }
        };

        struct PointLightData
        {
            alignas(16) glm::vec3 position = glm::vec3(0.0f);
            alignas(16) glm::vec3 color    = glm::vec3(0.0f);
            alignas(16) f32 radius       = 0.0f;
            alignas(16) f32 constant     = 0.0f;
            alignas(16) f32 linear       = 0.0f;
            alignas(16) f32 quadratic    = 0.0f;

            template<class Archive>
            void serialize(Archive& archive)
            {
                archive(cereal::make_nvp("position", position));
                archive(cereal::make_nvp("color", color));
                archive(cereal::make_nvp("radius", radius));
                archive(cereal::make_nvp("constant", constant));
                archive(cereal::make_nvp("linear", linear));
                archive(cereal::make_nvp("quadratic", quadratic));
            }
        };

        struct SpotLightData
        {
            // TODO: To be implemented
            alignas(16) glm::vec3 position  = glm::vec3(0.0f);
            alignas(16) glm::vec3 direction = glm::vec3(0.0f);
            alignas(16) glm::vec3 color     = glm::vec3(0.0f);
            alignas(16) f32 coneRadius    = 0.0f;
            alignas(16) f32 constant      = 0.0f;
            alignas(16) f32 linear        = 0.0f;
            alignas(16) f32 quadratic     = 0.0f;

            template<class Archive>
            void serialize(Archive& archive)
            {
                archive(cereal::make_nvp("position", position));
                archive(cereal::make_nvp("direction", direction));
                archive(cereal::make_nvp("color", color));
                archive(cereal::make_nvp("coneRadius", coneRadius));
                archive(cereal::make_nvp("constant", constant));
                archive(cereal::make_nvp("linear", linear));
                archive(cereal::make_nvp("quadratic", quadratic));
            }
        };
#endif

        // TODO: Add methods to render world icons in editor that can be used by the debug renderer
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

            RAZIX_INLINE glm::vec3 getColor() { return m_LightData.color; }
            void                   setColor(glm::vec3 color) { m_LightData.color = color; }

            RAZIX_INLINE glm::vec3 getDirection() { return glm::vec3(m_LightData.direction); }
            void                   setDirection(const glm::vec3& direction) { m_LightData.direction = glm::vec4(direction, 1.0f); }

            RAZIX_INLINE f32 getRadius() { return m_LightData.range; }
            void             setRadius(f32 radius) { m_LightData.range = radius; }

            RAZIX_INLINE const glm::vec3& getPosition() const { return m_LightData.position; }

            template<class Archive>
            void serialize(Archive& archive)
            {
                archive(cereal::make_nvp("type", m_LightData.type));
                archive(cereal::make_nvp("position", m_LightData.position));
                archive(cereal::make_nvp("range", m_LightData.range));
                archive(cereal::make_nvp("direction", m_LightData.direction));
                archive(cereal::make_nvp("color", m_LightData.color));
                archive(cereal::make_nvp("intensity", m_LightData.intensity));
                archive(cereal::make_nvp("constant", m_LightData.constant));
                archive(cereal::make_nvp("linear", m_LightData.linear));
                archive(cereal::make_nvp("quadratic", m_LightData.quadratic));
                archive(cereal::make_nvp("innerConeAngle", m_LightData.innerConeAngle));
                archive(cereal::make_nvp("outerConeAngle", m_LightData.outerConeAngle));
            }

        private:
            LightData m_LightData;
        };
    }    // namespace Graphics
}    // namespace Razix