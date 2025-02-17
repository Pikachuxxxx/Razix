#pragma once

#include "Razix/Core/RZHandle.h"

namespace Razix {
    namespace Gfx {

        // Forward declaration
        // RHI
        class RZTexture;
        class RZVertexBuffer;
        class RZIndexBuffer;
        class RZStorageBuffer;
        class RZUniformBuffer;
        class RZPipeline;
        class RZShader;
        class RZDrawCommandBuffer;
        class RZCommandPool;
        class RZDescriptorSet;
        class RZSampler;

        // Non-RHI
        class RZMesh;
        class RZMaterial;
        struct RZDecal;

        // Handles for Engine API types (Graphics)
        using RZTextureHandle           = RZHandle<Gfx::RZTexture>;
        using RZVertexBufferHandle      = RZHandle<Gfx::RZVertexBuffer>;
        using RZIndexBufferHandle       = RZHandle<Gfx::RZIndexBuffer>;
        using RZStorageBufferHandle     = RZHandle<Gfx::RZStorageBuffer>;
        using RZUniformBufferHandle     = RZHandle<Gfx::RZUniformBuffer>;
        using RZPipelineHandle          = RZHandle<Gfx::RZPipeline>;
        using RZShaderHandle            = RZHandle<Gfx::RZShader>;
        using RZDrawCommandBufferHandle = RZHandle<Gfx::RZDrawCommandBuffer>;
        using RZCommandPoolHandle       = RZHandle<Gfx::RZCommandPool>;
        using RZDescriptorSetHandle     = RZHandle<Gfx::RZDescriptorSet>;
        using RZSamplerHandle           = RZHandle<Gfx::RZSampler>;
        // High-level graphics primitives Handles
        using RZMeshHandle     = RZHandle<Gfx::RZMesh>;
        using RZMaterialHandle = RZHandle<Gfx::RZMaterial>;
        using RZDecalHandle    = RZHandle<Gfx::RZDecal>;
        // Null Handle
        using RZNullHandle = RZHandle<void>;

        // Constants
        constexpr u32 kInvalidSetIdx = ~0;

        // This is the only exception of using enum
        /* The stage which the shader corresponds to in the graphics pipeline */
        enum ShaderStage : u32
        {
            kNone                  = 0,
            kVertex                = 1 << 0,
            kPixel                 = 1 << 1,
            kCompute               = 1 << 2,
            kGeometry              = 1 << 3,
            kTesselationControl    = 1 << 4,
            kTesselationEvaluation = 1 << 5,
            kAmplification         = 1 << 6,
            kMesh                  = 1 << 7,
            ShaderStage_COUNT      = 6
        };

        /* Used the engine to find the right shader cache based on shader file name, forward declaring future API feature */
        enum class ShaderSourceType
        {
            NONE = -1,
            GLSL,
            SPIRV,
            HLSL,
            PSSL,
            CG
        };

        enum class ClearColorPresets : u32
        {
            OpaqueBlack,
            OpaqueWhite,
            TransparentBlack,
            TransparentWhite,
            Pink,
            DepthZeroToOne,
            DepthOneToZero,
            COUNT
        };

        /* What type of data does the descriptor set member represent */
        enum class DescriptorType : u32
        {
            kNone          = UINT32_MAX,
            kUniformBuffer = 0,
            kPushConstant,
            kImageSamplerCombined,    // (combined image sampler, Vulkan only) // Not Recommended for usage!
            kTexture,
            kRWTexture,
            kSampler,
            kRWTyped,    // ??? IDK why/what this is, I think this the same as RWDataBuffer in PSSL, takes in generic data types
            kStructured,
            kRWStructured,
            kByteAddress,
            kRWByteAddress,
            kAppendStructured,
            kConsumeStructured,
            kRWStructuredCounter,
            kRTAccelerationStructure,
            COUNT
        };

        /* The format of the input variables in the shader */
        enum class VertexInputFormat : u32
        {
            R8_UINT,
            R32_UINT,
            R32G32_UINT,
            R32G32B32_UINT,
            R32G32B32A32_UINT,
            R32_INT,
            R32G32_INT,
            R32G32B32_INT,
            R32G32B32A32_INT,
            R32_float,
            R32G32_float,
            R32G32B32_float,
            R32G32B32A32_float
        };

        /* The shader data type */
        enum class ShaderDataType : u32
        {
            NONE,
            FLOAT32,
            VEC2,
            VEC3,
            VEC4,
            IVEC2,
            IVEC3,
            IVEC4,
            MAT3,
            MAT4,
            INT32,
            INT,
            UINT,
            BOOL,
            STRUCT,
            MAT4ARRAY
        };

        enum class DescriptorHeapType
        {
            kCbvUavSrvHeap,
            kSamplerHeap,
            kRenderTargetHeap,
            kDepthStencilHeap,
            COUNT
        };

        /* The type of the texture */
        enum class TextureType
        {
            k1D = 0,
            k2D,
            k2DArray,
            k3D,
            kRW1D,
            kRW2D,
            kRW2DArray,
            kRW3D,
            kDepth,
            kCubeMap,
            kRWCubeMap,    // SRV is a CubeMap and UAV will be a RW2DArray view, special case handling of image views in backend
            kCubeMapArray,
            kSwapchainImage,
            COUNT
        };

        /* The format of the Texture resource */
        enum class TextureFormat
        {
            R8,
            R32_INT,
            R32_UINT,
            R32F,
            RG8,
            RG16F,
            RGB8,
            RGBA8,
            RGB16,
            RGBA16,
            RGBA16F,
            RGB32,
            RGBA32,
            RGBA32F,
            RGB,
            RGBA,
            DEPTH16_UNORM,
            DEPTH32F,
            STENCIL,
            DEPTH_STENCIL,
            SCREEN,
            BGRA8_UNORM,
            R11G11B10A2_UINT,
            R11G11B10A2_SFLOAT,
            NONE,
            COUNT
        };

        /* Engine internal format of the buffer data */
        enum class BufferFormat
        {
            INVALID,
            R8_INT,   /* char*            */
            R8_UINT,  /* unsigned char*   */
            R32_INT,  /* 32-bit integer   */
            R32_UINT, /* 32-bit uint      */
            R32_FLOAT,
            R32G32_INT,
            R32G32_UINT,
            R32G32_FLOAT,
            R32G32B32_INT,
            R32G32B32_UINT,
            R32G32B32_FLOAT,
            R32G32B32A32_INT,
            R32G32B32A32_UINT,
            R32G32B32A32_FLOAT,
            COUNT
        };

        /* Wrap mode for the texture texels */
        enum class Wrapping
        {
            kRepeat,
            kMirroredRepeat,
            kClampToEdge,
            kClampToBorder,
            COUNT
        };

        enum class PipelineType
        {
            kGraphics,
            kCompute
        };

        /* Culling mode describes which face of the polygon will be culled */
        enum class CullMode
        {
            Back,    // default
            Front,
            FrontBack,
            None,
            COUNT
        };

        /* Polygon mode describes how the geometry will be drawn, not the primitive used to draw */
        enum class PolygonMode
        {
            Fill,  /* The geometry will be filled with pixels                                                  */
            Line,  /* Only the outline of the geometry primitives will be drawn based on the line width set    */
            Point, /* Only the vertices will be drawn with square shaped points based on point size            */
            COUNT
        };

        /* Draw type describes what primitive will be used to draw the vertex data */
        enum class DrawType
        {
            Point,
            Triangle,
            Line,
            COUNT
        };

        /* Render Targets color blending function */
        enum class BlendOp
        {
            Add,
            Subtract,
            ReverseSubtract,
            Min,
            Max,
            COUNT
        };

        /* Blend Function factor */
        enum class BlendFactor
        {
            Zero,
            One,
            SrcColor,
            OneMinusSrcColor,
            DstColor,
            OneMinusDstColor,
            SrcAlpha,
            OneMinusSrcAlpha,
            DstAlpha,
            OneMinusDstAlpha,
            ConstantColor,
            OneMinusConstantColor,
            ConstantAlpha,
            OneMinusConstantAlpha,
            SrcAlphaSaturate,
            COUNT
        };

        /* Compare Operation Function for Depth and Stencil tests */
        enum class CompareOp
        {
            Never,
            Less,
            Equal,
            LessOrEqual,
            Greater,
            NotEqual,
            GreaterOrEqual,
            Always,
            COUNT
        };

        /* Defines how the buffer is used, enum cause of multiple flags for customization */
        enum class BufferUsage : u32
        {
            Static,           /* GPU only device memory, not CPU accessible, we can copy from GPU<->GPU though ig?, uses a staging buffer to copy any initial data */
            PersistentStream, /* GPU<->CPU two way mappable, but for for continuous updates, sequential updates                                                    */
            Staging,          /* Intermediate buffer for copying data from one Host to Device memory, Host visible                                                 */
            IndirectDrawArgs, /* GPU buffer for issuing indirect draw arguments                                                                                    */
            ReadBack,         /* Buffer to read back from GPU to CPU side                                                                                          */
            COUNT
        };

        /* Buffer Usage Additional flags for memory types */
        enum class BufferUsageAFlags
        {
            // These are more explicit types
            GPUOnly = 0,
            CPUOnly,
            CPUToGPU,
            GPUToCPU,
            GPUCopy,
            GPULazyAlloc,
            GenericAndPersistentlyMappableSlow,
            // These are more detailed usage types
            GPUCached,
            CPUCached,
            CPUCoherent,
            GPUCoherent,
            CPUSequentialWrite,
            CPUSequentialRead,
            CPURandomWrite,
            CPURandomRead,
            TransferHelp,
            COUNT
        };

        /* TODO: Useful for a unified buffer class similar to how RZTexture is */
        enum class BufferType
        {
            Constant,
            RWConstant,
            Storage,
            RWStructured,
            RWData,
            RWRegular,
            Structured,
            Data,
            Regular,
            AccelerationStructure,
            COUNT
        };

        /* The order of enums is how the GPU execution takes place */
        enum PipelineStage : u32
        {
            kTopOfPipe = 0,
            kDrawIndirect,
            kDraw,
            kVertexInput,
            kVertexShader,
            kTessellationControlShader,
            kTessellationEvaluationShader,
            kGeometryShader,
            kFragmentShader,
            kEarlyFragmentTests,
            kLateFragmentTests,
            kEarlyOrLateTests,
            kColorAttachmentOutput,
            kComputeShader,
            kTransfer,
            kMeshShader,
            kTaskShader,
            kBottomOfPipe,
            PipelineStage_COUNT
        };

        enum MemoryAccessMask : u32
        {
            kMemoryAccessNone = 0,
            kIndirectCommandReadBit,
            kIndexBufferDataReadBit,
            kVertexAttributeReadBit,
            kUniformReadBit,
            kInputAttachmentReadBit,
            kShaderReadBit,
            kShaderWriteBit,
            kColorAttachmentReadBit,
            kColorAttachmentWriteBit,
            kColorAttachmentReadWriteBit,
            kDepthStencilAttachmentReadBit,
            kDepthStencilAttachmentWriteBit,
            kTransferReadBit,
            kTransferWriteBit,
            kHostReadBit,
            kHostWriteBit,
            kMemoryReadBit,
            kMemoryWriteBit,
            MemoryAccessMask_COUNT
        };

        enum class ImageLayout : u32
        {
            kNewlyCreated,
            kGeneric,
            kSwapchain,
            kColorRenderTarget,
            kDepthRenderTarget,
            kDepthStencilRenderTarget,
            kDepthStencilReadOnly,
            kShaderRead,
            kShaderWrite,
            kTransferSource,
            kTransferDestination,
            COUNT
        };

        enum class DrawDataType
        {
            FLOAT,
            UNSIGNED_INT,
            UNSIGNED_BYTE
        };

        /* Rendering Resolution of the render targets */
        enum class Resolution : u32
        {
            k1080p = 0,  /* native 1920x1080 full HD resolution                                 */
            k1440p,      /* native 2K resolution 2560x1440 rendering                            */
            k4KUpscaled, /* Upscaled using FSR/DLSS                                             */
            k4KNative,   /* native 3840x2160 rendering                                          */
            kWindow,     /* Selects the resolution dynamically based on the presentation window */
            kCustom,     /* Custom resolution for rendering                                     */
            COUNT
        };

        // TODO: Add presets to select blending like Additive, Subtractive etc as in PhotoShop + util methods
        enum class BlendPresets
        {
            Additive,
            AlphaBlend,
            Subtractive,
            Multiply,
            Darken
        };

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

        /**
         * Flags passed to the Frame Graph Resource during preRead & preWrite
         * 
         * Thanks to Dawid for sharing these references and suggestions during our talks
         * Ref Source: https://github.com/skaarj1989/SupernovaEngine/blob/57f0fba69b36de45255971080ea082bea8219cbb/modules/Renderer/WorldRenderer/src/FrameGraphResourceAccess.hpp (MIT License)
         */

        /**
         * So we can either bind a resource as attachment or pass the Binding info struct 
         * Razix already has these 2 structs in the Graphics API everywhere
         * 
         * These structs have been loaned from RZDescriptorSet.h and RHI.h and
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

        /**
         * Rendering info is used by the GPU know the RTs, DRTs and viewport info etc.
         */
        struct RenderingInfo
        {
            Resolution                                                          resolution       = Resolution::kCustom;
            glm::uvec2                                                          extent           = {0, 0};
            std::vector<std::pair<RZTextureHandle, RenderTargetAttachmentInfo>> colorAttachments = {};
            std::pair<RZTextureHandle, RenderTargetAttachmentInfo>              depthAttachment  = {};
            int                                                                 layerCount       = 1;
            bool                                                                resize           = false;
        };

        /* Command Queue is a collection of command buffers that will be submitted for execution at once */
        typedef std::vector<RZDrawCommandBufferHandle> CommandQueue;

        /* Filtering for the Texture */
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

        struct PipelineBarrierInfo
        {
            PipelineStage startExecutionStage;
            PipelineStage endExecutionStage;
        };

        struct ImageMemoryBarrierInfo
        {
            MemoryAccessMask srcAccess;
            MemoryAccessMask dstAccess;
            ImageLayout      srcLayout;
            ImageLayout      dstLayout;
        };

        struct BufferMemoryBarrierInfo
        {
            MemoryAccessMask srcAccess;
            MemoryAccessMask dstAccess;
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

        struct RAZIX_MEM_ALIGN_16 RZDescriptor
        {
            std::string name     = "$DESCRIPTOR_UNNAMED";
            std::string typeName = "$DESCRIPTOR_TYPE_UNNAMED";
            union
            {
                RZUniformBufferHandle uniformBuffer = {};
                RZTextureHandle       texture;
                RZSamplerHandle       sampler;
            };
            std::vector<RZShaderBufferMemberInfo> uboMembers  = {};
            DescriptorBindingInfo                 bindingInfo = {};
            u32                                   size        = 0;
            u32                                   offset      = 0;
            ///////////////////////////////////////////////////

            // since RZHandle has custom copy and move constructors and operators, it make RZHandle non-trivially copyable
            // so we need to define copy constructors for the union to work and make this type trivial again
            RZDescriptor() {}
            RZDescriptor(const RZDescriptor& other);
            RZDescriptor& operator=(const RZDescriptor& other);
        };

        struct RAZIX_MEM_ALIGN_16 RZPushConstant
        {
            std::string                           typeName      = "$PUSH_CONSTANT_TYPE_UNNAMED";
            std::string                           name          = "$PUSH_CONSTANT_UNNAMED";
            std::vector<RZShaderBufferMemberInfo> structMembers = {};
            void*                                 data          = nullptr;
            DescriptorBindingInfo                 bindingInfo   = {};
            u32                                   size          = 0;
            u32                                   offset        = 0;
            ShaderStage                           shaderStage   = ShaderStage(0);
            u32                                   _padding      = 0;
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
            bool        enableAnisotropy = true;

            RZSamplerDesc(const std::string& name             = "$UNNAMED_SAMPLER",
                Wrapping                     wrapping         = Wrapping::kRepeat,
                Filtering                    filtering        = Filtering{},
                f32                          minLOD           = 0.0f,
                f32                          maxLOD           = 1.0f,
                f32                          maxAnisotropy    = 1.0f,
                bool                         enableAnisotropy = false)
                : name(name),
                  wrapping(wrapping),
                  filtering(filtering),
                  minLOD(minLOD),
                  maxLOD(maxLOD),
                  maxAnisotropy(maxAnisotropy),
                  enableAnisotropy(enableAnisotropy)
            {
            }
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
            void push<int8_t>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R8_INT, sizeof(int8_t), normalized);
            }

            template<>
            void push<u8>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R8_UINT, sizeof(u8), normalized);
            }

            template<>
            void push<int32_t>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R32_INT, sizeof(int32_t), normalized);
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
            void push<glm::vec2>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R32G32_FLOAT, sizeof(f32) * 2, normalized);
            }

            template<>
            void push<glm::vec3>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R32G32B32_FLOAT, sizeof(f32) * 3, normalized);
            }

            template<>
            void push<glm::vec4>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R32G32B32A32_FLOAT, sizeof(f32) * 4, normalized);
            }

            template<>
            void push<glm::ivec2>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R32G32_INT, sizeof(int) * 2, normalized);
            }

            template<>
            void push<glm::ivec3>(const std::string& name, bool normalized)
            {
                pushImpl(name, BufferFormat::R32G32B32_INT, sizeof(int) * 3, normalized);
            }

            template<>
            void push<glm::ivec4>(const std::string& name, bool normalized)
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

        struct RZDescriptorSetDesc
        {
            std::string               name = "$UNNAMED_DESCRIPTOR_HEAP"; /* Name of the descriptor heap */
            DescriptorHeapType        heapType;
            std::vector<RZDescriptor> descriptors;
            u32                       setIdx = kInvalidSetIdx;    // Set by the Shader Reflection, for util purposes only
        };

        /* Information necessary to create the pipeline */
        struct RZPipelineDesc
        {
            std::string                name                   = "$UNNAMED_PIPELINE_RESOURCE";
            PipelineType               pipelineType           = PipelineType::kGraphics;
            RZShaderHandle             shader                 = {};
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
        struct Drawable
        {
            RZMaterialHandle material;
            RZMeshHandle     mesh;
            uint32_t         transformID;
        };

        using Drawables = std::vector<Drawable>;

        /**
         * Drawables are batched by their common VB/IB and material pools
         */
        struct Batch
        {
            RZVertexBufferHandle vertexBuffer;
            RZIndexBufferHandle  indexBuffer;
            RZPipelineHandle     pso;
        };

        using Batches = std::vector<Batch>;

        /**
         * DrawData is use for Bindless Rendering 
         */
        struct DrawData
        {
            u32 drawBatchIdx  = 0;
            u32 drawableIdx   = 0;
            u32 vertexCount   = 0;
            u32 vertexOffset  = 0;
            u32 indexCount    = 0;
            u32 indexOffset   = 0;
            u32 instanceCount = 0;
            u32 _padding      = 0;
        };

        /**
         * Used to cache render commands for drawing batches b/w frames.
         */
        struct DrawCommandLists
        {
        };

        struct TextureReadback
        {
            void*    data;
            uint32_t width;
            uint32_t height;
            uint32_t bits_per_pixel;
        };

        //-----------------------------------------------------------------------------------
        // [Source] : https://twitter.com/SebAaltonen/status/1597135035811106816

        static std::unordered_map<ShaderStage, const char*> g_ShaderStageEntryPointNameMap = {
            {ShaderStage::kVertex, "VS_MAIN"},
            {ShaderStage::kPixel, "PS_MAIN"},
            {ShaderStage::kCompute, "CS_MAIN"},
            {ShaderStage::kGeometry, "GS_MAIN"}};

        static std::unordered_map<Resolution, glm::uvec2> g_ResolutionToExtentsMap = {
            {Resolution::k1080p, glm::uvec2(1920, 1080)},
            {Resolution::k1440p, glm::uvec2(2560, 1440)},
            {Resolution::k4KUpscaled, glm::uvec2(3840, 2160)},
            {Resolution::k4KNative, glm::uvec2(3840, 2160)}};

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
        glm::vec4                  ClearColorFromPreset(ClearColorPresets preset);

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

    }    // namespace Gfx
}    // namespace Razix
