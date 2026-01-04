#pragma once

//#include "Razix/Scene/RZSceneCamera.h"

#include "Razix/Asset/RZLightAsset.h"

namespace Razix {
    namespace Gfx {

        // Per‑Frame resources
#if LIMBO_STATE
        // Scene and G‑Buffer passes

        struct GBufferData
        {
            RZFrameGraphResource GBuffer0;
            RZFrameGraphResource GBuffer1;
            RZFrameGraphResource GBuffer2;
            RZFrameGraphResource GBufferDepth;
        };

        struct VisBufferData
        {
            RZFrameGraphResource visBuffer;    // Primitive/Instance/Draw/Alpha mask
            RZFrameGraphResource sceneDepth;
        };

        // Lighting & shadows

        struct SceneLightsData
        {
            RZFrameGraphResource lightsDataBuffer;
        };

        struct GPULightsData
        {
            alignas(4) u32 numLights   = 0;
            alignas(4) u32 _padding[3] = {0, 0, 0};
            alignas(16) LightData lightData[MAX_LIGHTS];
        };

        struct SimpleShadowPassData
        {
            RZFrameGraphResource shadowMap;
            RZFrameGraphResource lightVP;
        };

        struct CSMData
        {
            RZFrameGraphResource cascadedShadowMaps;
            RZFrameGraphResource viewProjMatrices;
        };

        constexpr u32 kShadowMapSize = 4096;
        constexpr u32 kNumCascades   = 4;
        static_assert(kNumCascades <= 4);
        constexpr f32 kSplitLambda = 0.81f;

        // bias matrix for shadow UV transform
        // clang-format off
        const float4x4 kBiasMatrix
        {
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        };
        // clang-format on

        struct alignas(4) CascadesMatrixData
        {
            float4   splitDepth                     = {};
            float4x4 viewProjMatrices[kNumCascades] = {};
        };

        struct alignas(16) Cascade
        {
            f32      splitDepth     = 0.0f;
            float4x4 viewProjMatrix = float4x4(1.0f);
        };

        // Volumetrics

        struct VolumetricCloudsData
        {
            RZFrameGraphResource noiseTexture;
        };

        // Post Processing

        struct SSAOImportData
        {
            RZFrameGraphResource SSAONoiseTexture;
            RZFrameGraphResource SSAOKernelSamples;
        };

        struct SSAOData
        {
            RZFrameGraphResource SSAOPreBlurTexture;
            RZFrameGraphResource SSAOSceneTexture;
            RZFrameGraphResource SSAOParams;
        };

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
#endif
        struct BRDFData
        {
            RZFrameGraphResource lut;
        };

        struct FrameData
        {
            RZFrameGraphResource frameData;
        };

        struct GPUCameraFrameData
        {
            alignas(16) float4x4 projection;
            alignas(16) float4x4 inversedProjection;
            alignas(16) float4x4 view;
            alignas(16) float4x4 inversedView;
            alignas(16) float4x4 prevViewProj;
            alignas(4) f32 fov;
            alignas(4) f32 nearPlane;
            alignas(4) f32 farPlane;
            alignas(4) f32 _padding;
        };

        struct GPUFrameData
        {
            f32                time;
            f32                deltaTime;
            uint2              resolution;
            GPUCameraFrameData camera;
            u32                renderFeatures;
            u32                debugFlags;
            float2             jitterTAA;
            float2             previousJitterTAA;
        };

        // Lighting
        struct SceneLightsData
        {
            RZFrameGraphResource lightsDataBuffer;
        };

        struct GPULightsData
        {
            alignas(4) u32 numLights = 0;
            alignas(4) u32 _pad0[3]  = {0, 0, 0};
            alignas(16) RZLightAsset lightAssets[RAZIX_MAX_LIGHTS_PER_WORLD];
        };

        // PBR/IBL
        struct LightProbe
        {
            rz_gfx_texture_handle skybox;
            rz_gfx_texture_handle diffuse;
            rz_gfx_texture_handle specular;
        };

        struct GlobalLightProbeData
        {
            RZFrameGraphResource environmentMap;
            RZFrameGraphResource diffuseIrradianceMap;
            RZFrameGraphResource specularPreFilteredMap;
        };

        struct SceneData
        {
            RZFrameGraphResource HDR;
            RZFrameGraphResource LDR;
            RZFrameGraphResource depth;
        };

        struct SkyboxPassData
        {
            RZFrameGraphResource SceneHDR;
        };

        struct ImGuiPassData
        {
            RZFrameGraphResource imguiRT;
            RZFrameGraphResource imguiDepth;
        };

        struct DebugPassData
        {
            RZFrameGraphResource DebugRT;
            RZFrameGraphResource DebugDRT;
        };

    }    // namespace Gfx
}    // namespace Razix
