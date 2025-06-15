#pragma once

#include "Razix/Gfx/Lighting/LightData.h"

namespace Razix {
    namespace Gfx {

        /**
         * Global light probes for PBR lighting
         */
        struct LightProbe
        {
            RZTextureHandle skybox;
            RZTextureHandle diffuse;
            RZTextureHandle specular;
        };

        struct GlobalLightProbeData
        {
            Razix::Gfx::RZFrameGraphResource environmentMap;
            Razix::Gfx::RZFrameGraphResource diffuseIrradianceMap;
            Razix::Gfx::RZFrameGraphResource specularPreFilteredMap;
        };

        struct VolumetricCloudsData
        {
            Razix::Gfx::RZFrameGraphResource noiseTexture;
        };

        // Default pass data types

        struct SceneData
        {
            RZFrameGraphResource sceneHDR;
            RZFrameGraphResource sceneLDR;
            RZFrameGraphResource sceneDepth;
        };

        struct SceneLightsData
        {
            RZFrameGraphResource lightsDataBuffer;
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

        /**
         * Simple shadow map data
         */
        struct SimpleShadowPassData
        {
            RZFrameGraphResource shadowMap;    // Depth texture to store the shadow map data
            RZFrameGraphResource lightVP;
        };

        struct VisBufferData
        {
            RZFrameGraphResource visBuffer;    // stores the Primitive_ID, Instance_ID/Draw_ID & alpha mask in a u32 screen space map
            RZFrameGraphResource sceneDepth;
        };

        namespace FX {

            struct SSAOData
            {
                RZFrameGraphResource SSAOPreBlurTexture;
                RZFrameGraphResource SSAOSceneTexture;
                RZFrameGraphResource SSAOParams;
            };

            struct SSAOImportData
            {
                RZFrameGraphResource SSAONoiseTexture;
                RZFrameGraphResource SSAOKernelSamples;
            };

            /**
             * Color Grading LUT
             */
            struct ColorGradingLUTData
            {
                RZFrameGraphResource neutralLUT;
            };

            struct ColorGradingData
            {
                RZFrameGraphResource colorGradedSceneHDR;
                RZFrameGraphResource colorGradedSceneLDR;
            };

            struct TAAResolveData
            {
                RZFrameGraphResource sourceTexture;
                RZFrameGraphResource historyTexture;
                RZFrameGraphResource accumulationTexture;
            };
        }    // namespace FX
    }    // namespace Gfx
}    // namespace Razix