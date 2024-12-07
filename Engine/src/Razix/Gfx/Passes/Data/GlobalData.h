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
            Razix::Gfx::FrameGraph::RZFrameGraphResource environmentMap;
            Razix::Gfx::FrameGraph::RZFrameGraphResource diffuseIrradianceMap;
            Razix::Gfx::FrameGraph::RZFrameGraphResource specularPreFilteredMap;
        };

        struct VolumetricCloudsData
        {
            Razix::Gfx::FrameGraph::RZFrameGraphResource noiseTexture;
        };

        // Default pass data types

        struct SceneData
        {
            FrameGraph::RZFrameGraphResource sceneHDR;
            FrameGraph::RZFrameGraphResource sceneLDR;
            FrameGraph::RZFrameGraphResource sceneDepth;
        };

        struct SceneLightsData
        {
            FrameGraph::RZFrameGraphResource lightsDataBuffer;
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
            FrameGraph::RZFrameGraphResource shadowMap;    // Depth texture to store the shadow map data
            FrameGraph::RZFrameGraphResource lightVP;
        };

        struct VisBufferData
        {
            FrameGraph::RZFrameGraphResource visBuffer;    // stores the Primitive_ID, Instance_ID/Draw_ID & alpha mask in a u32 screen space map
            FrameGraph::RZFrameGraphResource sceneDepth;
        };

        namespace FX {

            struct SSAOData
            {
                FrameGraph::RZFrameGraphResource SSAOPreBlurTexture;
                FrameGraph::RZFrameGraphResource SSAOSceneTexture;
                FrameGraph::RZFrameGraphResource SSAOParams;
            };

            struct SSAOImportData
            {
                FrameGraph::RZFrameGraphResource SSAONoiseTexture;
                FrameGraph::RZFrameGraphResource SSAOKernelSamples;
            };

            /**
             * Color Grading LUT
             */
            struct ColorGradingLUTData
            {
                FrameGraph::RZFrameGraphResource neutralLUT;
            };

            struct ColorGradingData
            {
                FrameGraph::RZFrameGraphResource colorGradedSceneHDR;
                FrameGraph::RZFrameGraphResource colorGradedSceneLDR;
            };

            struct TAAResolveData
            {
                FrameGraph::RZFrameGraphResource sourceTexture;
                FrameGraph::RZFrameGraphResource historyTexture;
                FrameGraph::RZFrameGraphResource accumulationTexture;
            };
        }    // namespace FX
    }    // namespace Gfx
}    // namespace Razix