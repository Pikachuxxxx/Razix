#pragma once

#include "Razix/Gfx/Resources/IRZResource.h"

namespace Razix {
    namespace Gfx {

        enum class SamplerPresets : u32
        {
            kDefaultGeneric = 0,
            kMipMap,
            kAnisotropic,
            kDiffuseTexSampler,
            kNormalMapSampler,
            kNonColorSampler,
            kColorSampler,
            COUNT
        };

        static RZSamplerDesc g_SamplerCreateDescPresets[u32(SamplerPresets::COUNT)] = {
            RZSamplerDesc(
                "DefaultGenericSampler",
                Wrapping::kRepeat,
                Filtering(Filtering::Mode::kFilterModeLinear, Filtering::Mode::kFilterModeLinear),
                0.0f,
                1.0f,
                1.0f,
                false),

            RZSamplerDesc(
                "MipMapSampler",
                Wrapping::kRepeat,
                Filtering(Filtering::Mode::kFilterModeLinear, Filtering::Mode::kFilterModeLinear),
                0.0f,
                1.0f,
                1.0f,
                false),

            RZSamplerDesc(
                "AnisotropicSampler",
                Wrapping::kClampToEdge,
                Filtering(Filtering::Mode::kFilterModeLinear, Filtering::Mode::kFilterModeLinear),
                0.0f,
                1.0f,
                16.0f,    // Enable high anisotropic filtering
                true),

            RZSamplerDesc(
                "DiffuseTexSampler",
                Wrapping::kRepeat,
                Filtering(Filtering::Mode::kFilterModeLinear, Filtering::Mode::kFilterModeLinear),
                0.0f,
                1.0f,
                1.0f,
                false),

            RZSamplerDesc(
                "NormalMapSampler",
                Wrapping::kRepeat,
                Filtering(Filtering::Mode::kFilterModeLinear, Filtering::Mode::kFilterModeLinear),
                0.0f,
                1.0f,
                1.0f,
                false),

            RZSamplerDesc(
                "NonColorSampler",
                Wrapping::kClampToBorder,
                Filtering(Filtering::Mode::kFilterModeNearest, Filtering::Mode::kFilterModeNearest),
                0.0f,
                1.0f,
                1.0f,
                false),

            RZSamplerDesc(
                "ColorSampler",
                Wrapping::kClampToEdge,
                Filtering(Filtering::Mode::kFilterModeLinear, Filtering::Mode::kFilterModeLinear),
                0.0f,
                1.0f,
                1.0f,
                false)};

        static RZSamplerHandle g_SamplerPresets[u32(SamplerPresets::COUNT)] = {};

        class RAZIX_API RZSampler : public IRZResource<RZSampler>
        {
        public:
            static void CreateSamplerPresets();
            static void DestroySamplerPresets();

        public:
            RZSampler() {}
            RAZIX_VIRTUAL_DESCTURCTOR(RZSampler)

            GET_INSTANCE_SIZE;

            inline const RZSamplerDesc&   getDesc() const { return m_Desc; }
            inline const Wrapping&        getWrapMode() const { return m_Desc.wrapping; }
            inline const Filtering&       getFilterMode() const { return m_Desc.filtering; }
            inline const Filtering::Mode& getMinFilterMode() const { return m_Desc.filtering.minFilter; }
            inline const Filtering::Mode& getMagFilterMode() const { return m_Desc.filtering.magFilter; }

        protected:
            RZSamplerDesc m_Desc;

        private:
            static void Create(void* where, const RZSamplerDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);

            // only resource manager can create an instance of this class
            friend class RZResourceManager;
        };
    }    // namespace Gfx
}    // namespace Razix