#pragma once

#include "Razix/Core/RZHandle.h"

#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {
    namespace Gfx {

#define RAZIX_MAX_SHADER_SOURCE_SIZE 1024 * 1024
#define RAZIX_MAX_LINE_LENGTH        1024
#define RAZIX_MAX_SHADER_STAGES      RZ_GFX_SHADER_STAGE_COUNT
#define RAZIX_MAX_INCLUDE_DEPTH      16

        /* Final target FPS */
        enum class TargetFPS
        {
            k60  = 60,
            k120 = 120
        };

        enum class ShaderBuiltin : u32
        {
            //------------------------------
            Default,
            Skybox,
            ProceduralSkybox,
            GBuffer,
            PBRDeferredLighting,
            //----------------
            VisibilityBufferFill,
            //----------------
            DeferredDecals,
            Composition,
            DepthPreTest,
            CSM,
            EnvToCubemap,
            GenerateIrradianceMap,
            GeneratePreFilteredMap,
            Sprite,
            SpriteTextured,
            DebugPoint,
            DebugLine,
            ImGui,
            // Post Processing FX
            SSAO,
            GaussianBlur,
            ColorGrading,
            TAAResolve,
            FXAA,
            Tonemap,
            //---
            COUNT
        };

        struct TextureReadback
        {
            void*    data;
            uint32_t width;
            uint32_t height;
            uint32_t bits_per_pixel;
        };

        rz_gfx_shader_desc       ParseRZSF(const std::string& filePath);
        void                     FreeRZSFBytecodeAlloc(rz_gfx_shader* shader);
        rz_gfx_shader_reflection ReflectShader(const rz_gfx_shader* shader);

#if 0
        /**
         * Flags passed to the Frame Graph Resource during preRead & preWrite
         * 
         * Thanks to Dawid for sharing these references and suggestions during our talks
         * Ref Source: https://github.com/skaarj1989/SupernovaEngine/blob/57f0fba69b36de45255971080ea082bea8219cbb/modules/Renderer/WorldRenderer/src/FrameGraphResourceAccess.hpp (MIT License)
         */

        /**
         * So we can either bind a resource as attachment or pass the Binding info struct 
         * Razix already has these 2 structs in the Graphics API everywhere
         */

        /**
         * We encode them into 32 bytes before we pass them off to framegraph as flags
         */

        struct RAZIX_MEM_ALIGN_16 BindingLocation
        {
            u32 set     = 0;
            u32 binding = 0;

            RAZIX_NO_DISCARD u32 encode();
            RAZIX_NO_DISCARD     operator u32() const;
        };

        struct RAZIX_MEM_ALIGN_16 DescriptorBindingInfo
        {
            DescriptorType  type;
            ShaderStage     stage;
            BindingLocation location = {};
            u32             count    = 1;

            RAZIX_NO_DISCARD u32 encode();
            RAZIX_NO_DISCARD     operator u32() const;
        };

        /* Gives information for the attachment Info */
        struct RAZIX_MEM_ALIGN_16 RenderTargetAttachmentInfo
        {
            bool              clear      = true;
            ClearColorPresets clearColor = ClearColorPresets::TransparentBlack;
            u32               bindingIdx = 0;
            u32               mip        = 0;
            u32               layer      = 0;

            RAZIX_NO_DISCARD u32 encode();
            RAZIX_NO_DISCARD     operator u32() const;
        };

        struct Filtering
        {
            enum class Mode
            {
                kFilterModeLinear,
                kFilterModeNearest,
                COUNT
            };
            Mode minFilter = Mode::kFilterModeNearest;
            Mode magFilter = Mode::kFilterModeNearest;

            Filtering() {}
            Filtering(Mode min, Mode max)
                : minFilter(min), magFilter(max) {}
        };

        struct RAZIX_MEM_ALIGN_16 RZVertexInputBindingInfo
        {
            u32               binding      = 0;
            u32               location     = 0;
            VertexInputFormat vertexFormat = VertexInputFormat::R8_UINT;
            u32               offset       = 0;
        };

        struct RAZIX_MEM_ALIGN_16 RZShaderBufferMemberInfo
        {
            std::string    name     = "";
            std::string    fullName = "";
            u32            size     = 0;
            u32            offset   = 0;
            ShaderDataType type     = ShaderDataType::INT;
            u32            _padding = 0;
        };

        struct RAZIX_API BufferLayoutElement
        {
            std::string  name;
            BufferFormat format;
            u32          offset     = 0;
            bool         normalized = false;

            u32 getCount();
        };

        //-----------------------------------------------------------------------------------
        // Graphics API Create Helpers
        // All API desc structs must have a name member variable

        // TODO: Add checks for data members based on type, ex. Texture_CubeMap must have layers > 1 etc.
        struct RZTextureDesc
        {
            std::string   name                  = "$UNNAMED_TEXTURE_RESOURCE";
            u32           width                 = 0;
            u32           height                = 0;
            u32           depth                 = 1;
            u32           layers                = 1;
            void*         data                  = nullptr;
            u32           size                  = 0;
            TextureType   type                  = TextureType::k2D;
            TextureFormat format                = TextureFormat::RGBA16F;
            bool          enableMips            = false;
            bool          flipX                 = false;
            bool          flipY                 = true;
            bool          ownsInitData          = true;
            bool          allowResize           = false;
            u8            initResourceViewHints = 1;
            u32           dataSize              = sizeof(unsigned char);
            std::string   filePath              = "";

            static RAZIX_API std::string FormatToString(const Gfx::TextureFormat format);
            static RAZIX_API std::string TypeToString(TextureType type);

            static RAZIX_API TextureFormat StringToFormat(const std::string& str);
            static RAZIX_API TextureType   StringToType(const std::string& str);
        };

        struct RZSamplerDesc
        {
            std::string name             = "$UNNAMED_SAMPLER";
            Wrapping    wrapping         = Wrapping::kRepeat;
            Filtering   filtering        = Filtering{};
            f32         minLOD           = 0.0f;
            f32         maxLOD           = 1.0f;
            f32         maxAnisotropy    = 1.0f;
            bool        enableAnisotropy = false;
        };

        /* Vertex buffer layout describes the layout of the buffer data that is being uploaded to the GPU to be consumed by the Input Assembler */
        struct RZBufferLayout
        {
            template<typename T>
            void push(const std::string& name, bool Normalised = false)
            {
                RAZIX_ASSERT(false, "Unkown buffer element layout type!");
            }

            template<>
            void push<i8>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R8_INT, sizeof(i8), normalized);
            }

            template<>
            void push<u8>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R8_UINT, sizeof(u8), normalized);
            }

            template<>
            void push<i32>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R32_INT, sizeof(i32), normalized);
            }

            template<>
            void push<u32>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R32_UINT, sizeof(u32), normalized);
            }

            template<>
            void push<f32>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R32_FLOAT, sizeof(f32), normalized);
            }

            template<>
            void push<float2>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R32G32_FLOAT, sizeof(f32) * 2, normalized);
            }

            template<>
            void push<float3>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R32G32B32_FLOAT, sizeof(f32) * 3, normalized);
            }

            template<>
            void push<float4>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R32G32B32A32_FLOAT, sizeof(f32) * 4, normalized);
            }

            template<>
            void push<int2>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R32G32_INT, sizeof(int) * 2, normalized);
            }

            template<>
            void push<int3>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R32G32B32_INT, sizeof(int) * 3, normalized);
            }

            template<>
            void push<int4>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R32G32B32A32_INT, sizeof(int) * 4, normalized);
            }

            inline std::vector<BufferLayoutElement>& getElements() { return m_Elements; }
            inline u32                               getStride() const { return m_Stride; }

        private:
            std::vector<BufferLayoutElement> m_Elements;
            u32                              m_Stride;

        private:
            void pushImpl(const std::string& name, BufferFormat format, u32 size, bool Normalised);
        };

        //-----------------------------------------------------------------------------------
        // API Create Desc

        /* Used for creating Vertex (VB_), Index(IB_) or Constant buffers(CB_) */
        struct RZBufferDesc
        {
            std::string name = "$UNNAMED_BUFFER";
            union
            {
                u32 size = 0;
                u32 count;
            };
            void*          data                  = nullptr;
            BufferUsage    usage                 = BufferUsage::Static;
            RZBufferLayout layout                = {};
            u8             initResourceViewHints = 0;
        };

        struct RZShaderDesc
        {
            std::string   name = "$UNNAMED_SHADER";
            std::string   filePath;
            ShaderBuiltin libraryID;
        };

        /* Information necessary to create the pipeline */
        struct RZPipelineDesc
        {
            std::string  name         = "$UNNAMED_PIPELINE_RESOURCE";
            PipelineType pipelineType = PipelineType::kGraphics;
            //RZShaderHandle             shader                 = {};
            std::vector<TextureFormat> colorAttachmentFormats = {};
            TextureFormat              depthFormat            = TextureFormat::DEPTH32F;
            CullMode                   cullMode               = CullMode::Front;
            PolygonMode                polygonMode            = PolygonMode::Fill;
            DrawType                   drawType               = DrawType::Triangle;
            bool                       transparencyEnabled    = true;
            bool                       depthBiasEnabled       = false;
            bool                       depthTestEnabled       = true;
            bool                       depthWriteEnabled      = true;
            BlendFactor                colorSrc               = BlendFactor::SrcAlpha;
            BlendFactor                colorDst               = BlendFactor::OneMinusSrcAlpha;
            BlendOp                    colorOp                = BlendOp::Add;
            BlendFactor                alphaSrc               = BlendFactor::One;
            BlendFactor                alphaDst               = BlendFactor::One;
            BlendOp                    alphaOp                = BlendOp::Add;
            CompareOp                  depthOp                = CompareOp::Less;
        };

        //-----------------------------------------------------------------------------------
        // High-Level Rendering helper structs

        // [Source]: https://github.com/skaarj1989/SupernovaEngine

        /**
         * Drawable is a conversion of an actor/entity that will be rendered onto the scene
         */
        // struct Drawable
        // {
        //     RZMaterialHandle material;
        //     RZMeshHandle     mesh;
        //     uint32_t         transformID;
        // };

        //using Drawables = std::vector<Drawable>;

        /**
         * Drawables are batched by their common VB/IB and material pools
         */
        //  struct Batch
        //  {
        //      RZVertexBufferHandle vertexBuffer;
        //      RZIndexBufferHandle  indexBuffer;
        //      RZPipelineHandle     pso;
        //  };

        //  using Batches = std::vector<Batch>;

        /**
         * DrawData is use for Bindless Rendering 
         */
        // struct DrawData
        // {
        //     u32 drawBatchIdx  = 0;
        //     u32 drawableIdx   = 0;
        //     u32 vertexCount   = 0;
        //     u32 vertexOffset  = 0;
        //     u32 indexCount    = 0;
        //     u32 indexOffset   = 0;
        //     u32 instanceCount = 0;
        //     u32 _padding      = 0;
        // };

        /**
         * Used to cache render commands for drawing batches b/w frames.
         */
        struct DrawCommandLists
        {
        };



        //-----------------------------------------------------------------------------------
        // [Source] : https://twitter.com/SebAaltonen/status/1597135035811106816

        static std::unordered_map<ShaderStage, const char*> g_ShaderStageEntryPointNameMap = {
            {ShaderStage::kVertex, "VS_MAIN"},
            {ShaderStage::kPixel, "PS_MAIN"},
            {ShaderStage::kCompute, "CS_MAIN"},
            {ShaderStage::kGeometry, "GS_MAIN"}};

        static std::unordered_map<Resolution, uint2> g_ResolutionToExtentsMap = {
            {Resolution::k1080p, uint2(1920, 1080)},
            {Resolution::k1440p, uint2(2560, 1440)},
            {Resolution::k4KUpscaled, uint2(3840, 2160)},
            {Resolution::k4KNative, uint2(3840, 2160)}};

        static const char* BufferBarrierTypeNames[] = {
            "CPUToGPU",
            "GPUToCPU",
            "TransferWriteToShaderRead",
            "ShaderReadToShaderWrite",
            "ShaderWriteToShaderRead",
            "ShaderReadOnly"};

        RAZIX_ENUM_NAMES_ASSERT(BufferBarrierTypeNames, BufferBarrierType);

        static const char* ImageLayoutNames[] =
            {
                "NewlyCreated",
                "Generic",
                "Swapchain",
                "ColorRenderTarget",
                "DepthRenderTarget",
                "DepthStencilRenderTarget",
                "DepthStencilReadOnly",
                "ShaderRead",
                "ShaderWrite",
                "TransferSource",
                "TransferDestination"};

        RAZIX_ENUM_NAMES_ASSERT(ImageLayoutNames, ImageLayout);

        static const char* ClearColorPresetsNames[] =
            {
                "OpaqueBlack",
                "OpaqueWhite",
                "TransparentBlack",
                "TransparentWhite",
                "Pink",
                "DepthZeroToOne",
                "DepthOneToZero"};

        RAZIX_ENUM_NAMES_ASSERT(ClearColorPresetsNames, ClearColorPresets);

        static const char* ResolutionNames[] =
            {
                "k1080p",
                "k1440p",
                "k4KUpscaled",
                "k4KNative",
                "kWindow",
                "kCustom"};

        RAZIX_ENUM_NAMES_ASSERT(ResolutionNames, Resolution);

        static const char* TextureTypeNames[] = {
            "Texture_1D",
            "Texture_2D",
            "Texture_2DArray",
            "Texture_3D",
            "Texture_RW1D",
            "Texture_RW2D",
            "Texture_RW2DArray",
            "Texture_RW3D",
            "Texture_Depth",
            "Texture_CubeMap",
            "Texture_RWCubeMap",
            "Texture_CubeMapArray",
            "Texture_SwapchainImage"};

        RAZIX_ENUM_NAMES_ASSERT(TextureTypeNames, TextureType);

        static const char* TextureFormatNames[] = {
            "R8",
            "R32_INT",
            "R32_UINT",
            "R32F",
            "RG8",
            "RG16F",
            "RGB8",
            "RGBA8",
            "RGB16",
            "RGBA16",
            "RGBA16F",
            "RGB32",
            "RGBA32",
            "RGBA32F",
            "RGB",
            "RGBA",
            "DEPTH16_UNORM",
            "DEPTH32F",
            "STENCIL",
            "DEPTH_STENCIL",
            "SCREEN",
            "BGRA8_UNORM",
            "R11G11B10A2_UINT",
            "R11G11B10A2_SFLOAT",
            "NONE"};

        RAZIX_ENUM_NAMES_ASSERT(TextureFormatNames, TextureFormat);

        static const char* WrappingNames[] = {
            "REPEAT",
            "MIRRORED_REPEAT",
            "CLAMP_TO_EDGE",
            "CLAMP_TO_BORDER"};

        RAZIX_ENUM_NAMES_ASSERT(WrappingNames, Wrapping);

        static const char* FitleringModeNames[] =
            {
                "FilterModeLinear",
                "FilterModeNearest"};

        RAZIX_ENUM_NAMES_ASSERT(FitleringModeNames, Filtering::Mode);

        static const char* CullModeNames[] = {
            "Back",
            "Front",
            "FrontBack",
            "None"};

        RAZIX_ENUM_NAMES_ASSERT(CullModeNames, CullMode);

        static const char* PolygonModeNames[] = {
            "Fill",
            "Line",
            "Point"};

        RAZIX_ENUM_NAMES_ASSERT(PolygonModeNames, PolygonMode);

        static const char* DrawTypeNames[] = {
            "Point",
            "Triangle",
            "Line"};

        RAZIX_ENUM_NAMES_ASSERT(DrawTypeNames, DrawType);

        static const char* BlendOpNames[] = {
            "Add",
            "Subtract",
            "ReverseSubtract",
            "Min",
            "Max"};

        RAZIX_ENUM_NAMES_ASSERT(BlendOpNames, BlendOp);

        static const char* BlendFactorNames[] = {
            "Zero",
            "One",
            "SrcColor",
            "OneMinusSrcColor",
            "DstColor",
            "OneMinusDstColor",
            "SrcAlpha",
            "OneMinusSrcAlpha",
            "DstAlpha",
            "OneMinusDstAlpha",
            "ConstantColor",
            "OneMinusConstantColor",
            "ConstantAlpha",
            "OneMinusConstantAlpha",
            "SrcAlphaSaturate"};

        RAZIX_ENUM_NAMES_ASSERT(BlendFactorNames, BlendFactor);

        static const char* CompareOpNames[] = {
            "Never",
            "Less",
            "Equal",
            "LessOrEqual",
            "Greater",
            "NotEqual",
            "GreaterOrEqual",
            "Always"};

        RAZIX_ENUM_NAMES_ASSERT(CompareOpNames, CompareOp);

        static const char* BufferUsageNames[] = {
            "Static",
            "PersistentStream",
            "Staging",
            "IndirectDrawArgs",
            "ReadBack"};

        // Can't do this because it's enum and we have multiple options
        RAZIX_ENUM_NAMES_ASSERT(BufferUsageNames, BufferUsage);

        static const char* BufferTypeNames[] =
            {
                "Constant",
                "RWConstant",
                "Storage",
                "RWStructured",
                "RWData",
                "RWRegular",
                "Structured",
                "Data",
                "Regular",
                "AccelerationStructure"};

        RAZIX_ENUM_NAMES_ASSERT(BufferTypeNames, BufferType);

        //-----------------------------------------------------------------------------------

        RAZIX_NO_DISCARD u32       EncodeBindingLocation(BindingLocation info);
        BindingLocation            DecodeBindingLocation(u32 bits);
        RAZIX_NO_DISCARD u32       EncodeDescriptorBindingInfo(DescriptorBindingInfo info);
        DescriptorBindingInfo      DecodeDescriptorBindingInfo(u32 bits);
        RAZIX_NO_DISCARD u32       EncodeAttachmentInfo(RenderTargetAttachmentInfo info);
        RenderTargetAttachmentInfo DecodeAttachmentInfo(u32 bits);
        float4                     ClearColorFromPreset(ClearColorPresets preset);

        static std::unordered_map<std::string, Razix::Gfx::Resolution> StringToResolutionsMap = {
            {"k1080p", Razix::Gfx::Resolution::k1080p},
            {"k1440p", Razix::Gfx::Resolution::k1440p},
            {"k4KUpscaled", Razix::Gfx::Resolution::k4KUpscaled},
            {"k4KNative", Razix::Gfx::Resolution::k4KNative},
            {"kWindow", Razix::Gfx::Resolution::kWindow},
            {"kCustom", Razix::Gfx::Resolution::kCustom}};

        static std::unordered_map<std::string, Razix::Gfx::ClearColorPresets> StringToColorPreset = {
            {"OpaqueBlack", Razix::Gfx::ClearColorPresets::OpaqueBlack},
            {"OpaqueWhite", Razix::Gfx::ClearColorPresets::OpaqueWhite},
            {"TransparentBlack", Razix::Gfx::ClearColorPresets::TransparentBlack},
            {"TransparentWhite", Razix::Gfx::ClearColorPresets::TransparentWhite},
            {"Pink", Razix::Gfx::ClearColorPresets::Pink},
            {"DepthZeroToOne", Razix::Gfx::ClearColorPresets::DepthZeroToOne},
            {"DepthOneToZero", Razix::Gfx::ClearColorPresets::DepthOneToZero}};

        /* utility functions for frame graph parsing to convert string to enums */
        CompareOp       StringToCompareOp(const std::string& str);
        DrawType        StringToDrawType(const std::string& str);
        PolygonMode     StringToPolygonMode(const std::string& str);
        CullMode        StringToCullMode(const std::string& str);
        BlendOp         StringToBlendOp(const std::string& str);
        TextureFormat   StringToTextureFormat(const std::string& str);
        TextureType     StringToTextureType(const std::string& str);
        Wrapping        StringToWrapping(const std::string& str);
        Filtering::Mode StringToFilteringMode(const std::string& str);
        BlendFactor     StringToBlendFactor(const std::string& str);
        BufferUsage     StringToBufferUsage(const std::string& str);
        ShaderStage     StringToShaderStage(const std::string& str);
#endif

    }    // namespace Gfx
}    // namespace Razix
