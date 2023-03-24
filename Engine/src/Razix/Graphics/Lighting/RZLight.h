#pragma once

namespace Razix {
    namespace Graphics {

        class RZUniformBuffer;

#define MAX_LIGHTS 128

        enum LightType : u32
        {
            DIRECTIONAL = 0,
            POINT       = 1,
            SPOT        = 2,
            AREA,    // Not Supported!
            FLASH    // Not Supported!
        };

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

        struct RAZIX_MEM_ALIGN LightData
        {
            alignas(16) glm::vec3 position  = glm::vec3(1.0f);
            alignas(4) f32 range          = 10.0f;
            alignas(16) glm::vec3 color     = glm::vec3(1.0f);
            alignas(4) f32 intensity      = 1.0f;
            alignas(16) glm::vec4 direction = glm::vec4(1.0f);
            alignas(4) f32 constant       = 1.0f;
            alignas(4) f32 linear         = 0.09f;
            alignas(4) f32 quadratic      = 0.032f;
            alignas(4) f32 innerConeAngle = 12.5f;    // [Spot]
            alignas(4) f32 outerConeAngle = 60.0f;    // [Spot]
            alignas(4) LightType type       = LightType::DIRECTIONAL;
            alignas(4) u32 _padding[2] = {0, 0};    // Implicit padding that will be consumed by GLSL for 16 byte alignment
        };

        /**
         * Lights Data which will be uploaded to the GPU
         */
        struct GPULightsData
        {
            alignas(4) u32 numLights   = 0;
            alignas(4) u32 _padding[3] = {0, 0, 0};    // Will be consumed on GLSL so as to get 16 byte alignment, invisible variable on GLSL
            alignas(16) LightData lightData[MAX_LIGHTS];
        };

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

            inline LightType getLightType() { return m_LightData.type; }

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