#pragma once

namespace Razix {
    namespace Graphics {

        class RZUniformBuffer;

#define MAX_LIGHTS 1024

        enum LightType : uint32_t
        {
            DIRECTIONAL,
            POINT,
            SPOT,
            AREA,
            FLASH
        };

        struct DirectionalLightData
        {
            alignas(16) glm::vec3 direction = glm::vec3(0.0f);
            alignas(16) glm::vec3 color     = glm::vec3(0.0f);
            alignas(16) float time          = 0.0f;

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
            alignas(16) float radius       = 0.0f;
            alignas(16) float constant     = 0.0f;
            alignas(16) float linear       = 0.0f;
            alignas(16) float quadratic    = 0.0f;

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
            alignas(16) float coneRadius    = 0.0f;
            alignas(16) float constant      = 0.0f;
            alignas(16) float linear        = 0.0f;
            alignas(16) float quadratic     = 0.0f;

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

        struct LightData
        {
            LightType            type = LightType::DIRECTIONAL;
            DirectionalLightData dirLightData{};
            PointLightData       pointLightData{};
            SpotLightData        spotLightData{};
        };

        // TODO: Add methods to render world icons in editor that can be used by the debug renderer
        class RAZIX_API RZLight
        {
        public:
        public:
            RZLight(LightType type = LightType::DIRECTIONAL);
            ~RZLight();

            void updateLight();

            LightData& getLightData() { return m_LightData; }

            void setLightProperties(DirectionalLightData& dirLightData);
            void setLightProperties(PointLightData& pointLightData);
            void setLightProperties(SpotLightData& sportLightData);

            void setLightType(LightType type);

            inline LightType getLightType() { return m_LightData.type; }

            template<class Archive>
            void serialize(Archive& archive)
            {
                archive(cereal::make_nvp("type", m_LightData.type));
                switch (m_LightData.type) {
                    case LightType::DIRECTIONAL:
                        archive(cereal::make_nvp("DirectionalLight", m_LightData.dirLightData));
                        break;
                    case LightType::POINT:
                        archive(cereal::make_nvp("PointLight", m_LightData.pointLightData));
                        break;
                    case LightType::SPOT:
                        archive(cereal::make_nvp("SpotLight", m_LightData.spotLightData));
                        break;
                }
            }

        private:
            //RZUniformBuffer*     m_LightUBO;
            LightData m_LightData;
        };
    }    // namespace Graphics
}    // namespace Razix