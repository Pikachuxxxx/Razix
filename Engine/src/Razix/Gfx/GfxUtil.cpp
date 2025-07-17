// clang-format off
#include "rzxpch.h"
// clang-format on
#include "GfxUtil.h"

#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Utilities/RZStringUtilities.h"

namespace Razix {
    namespace Gfx {

        static std::string ShaderBinaryFileExtension = "";
        static std::string ShaderBinaryFileDirectory = "";

        static const std::unordered_map<std::string, rz_gfx_shader_stage> kStageMap = {
            {"vertex", RZ_GFX_SHADER_STAGE_VERTEX},
            {"fragment", RZ_GFX_SHADER_STAGE_PIXEL},
            {"pixel", RZ_GFX_SHADER_STAGE_PIXEL},
            {"geometry", RZ_GFX_SHADER_STAGE_GEOMETRY},
            {"compute", RZ_GFX_SHADER_STAGE_COMPUTE},
            {"tesscontrol", RZ_GFX_SHADER_STAGE_TESSELLATION_CONTROL},
            {"tesseval", RZ_GFX_SHADER_STAGE_TESSELLATION_EVALUATION},
            {"mesh", RZ_GFX_SHADER_STAGE_MESH},
            {"task", RZ_GFX_SHADER_STAGE_TASK},
            {"raygen", RZ_GFX_SHADER_STAGE_RAY_GEN},
            {"miss", RZ_GFX_SHADER_STAGE_RAY_MISS},
            {"closesthit", RZ_GFX_SHADER_STAGE_RAY_CLOSEST_HIT},
            {"anyhit", RZ_GFX_SHADER_STAGE_RAY_ANY_HIT}};

        rz_gfx_shader_desc ParseRZSF(const std::string& filePath)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_CORE_TRACE("Parsing .rzsf shader : {0}", filePath);

            std::map<rz_gfx_shader_stage, std::string> shaders;
            std::vector<std::string>                   shader_defines;
            rz_gfx_shader_desc                         desc       = {0};
            std::string                                rzsfSource = RZVirtualFileSystem::Get().readTextFile(filePath);

            // Break the shader into lines
            std::vector<std::string> lines = Razix::Utilities::GetLines(rzsfSource);
            rz_gfx_shader_stage      stage = rz_gfx_shader_stage::RZ_GFX_SHADER_STAGE_NONE;

            switch (rzGfxCtx_GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_VULKAN
                case RZ_RENDER_API_VULKAN:
                    ShaderBinaryFileExtension = ".spv";
                    ShaderBinaryFileDirectory = "Compiled/SPIRV/";
                    break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case RZ_RENDER_API_D3D12:
                    ShaderBinaryFileExtension = ".cso";
                    ShaderBinaryFileDirectory = "Compiled/CSO/";
                    break;
#endif
                default: break;
            }

            for (const std::string& raw_line: lines) {
                std::string line = Utilities::TrimWhitespaces(raw_line);

                if (Razix::Utilities::StartsWith(line, "#shader")) {
                    for (const auto& [key, val]: kStageMap) {
                        if (Razix::Utilities::StringContains(line, key)) {
                            stage          = val;
                            shaders[stage] = "";    // initialize empty source for stage
                            break;
                        }
                    }
                } else if (Razix::Utilities::StartsWith(line, "#ifdef")) {
                    std::string condition = line.substr(7);    // skip "#ifdef "
                    condition             = Razix::Utilities::RemoveSpaces(condition);
                    auto defines          = Razix::Utilities::SplitString(condition, "||");
                    shader_defines.insert(shader_defines.end(), defines.begin(), defines.end());
                } else if (Razix::Utilities::StartsWith(line, "#include")) {
                    std::string includePath = Utilities::TrimWhitespaces(line.substr(9));    // skip "#include "
                    includePath += ShaderBinaryFileExtension;
                    std::string fullPath = ShaderBinaryFileDirectory + includePath;
                    shaders[stage] += fullPath;
                }
            }

            for (const auto& [stage, includePath]: shaders) {
                std::string virtualPath = "//RazixContent/Shaders/" + includePath;
                std::string outPath;

                // Resolve to actual file path
                RZVirtualFileSystem::Get().resolvePhysicalPath(virtualPath, outPath);
                uint32_t bytecodeSize = (uint32_t) RZFileSystem::GetFileSize(outPath);
                u8*      bytecode     = RZVirtualFileSystem::Get().readFile(virtualPath);

                if (!bytecode) {
                    RAZIX_CORE_ERROR("Failed to read shader bytecode: {0}", includePath);
                    continue;
                }

                void* heapCopy = Memory::RZMemCopyToHeap(bytecode, bytecodeSize);

                switch (stage) {
                    case RZ_GFX_SHADER_STAGE_VERTEX:
                        desc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.raster.vs.stage    = stage;
                        desc.raster.vs.bytecode = (const char*) heapCopy;
                        desc.raster.vs.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_PIXEL:
                        desc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.raster.ps.stage    = stage;
                        desc.raster.ps.bytecode = (const char*) heapCopy;
                        desc.raster.ps.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_GEOMETRY:
                        desc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.raster.gs.stage    = stage;
                        desc.raster.gs.bytecode = (const char*) heapCopy;
                        desc.raster.gs.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_TESSELLATION_CONTROL:
                        desc.pipelineType        = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.raster.tcs.stage    = stage;
                        desc.raster.tcs.bytecode = (const char*) heapCopy;
                        desc.raster.tcs.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_TESSELLATION_EVALUATION:
                        desc.pipelineType        = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.raster.tes.stage    = stage;
                        desc.raster.tes.bytecode = (const char*) heapCopy;
                        desc.raster.tes.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_COMPUTE:
                        desc.pipelineType        = RZ_GFX_PIPELINE_TYPE_COMPUTE;
                        desc.compute.cs.stage    = stage;
                        desc.compute.cs.bytecode = (const char*) heapCopy;
                        desc.compute.cs.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_MESH:
                        desc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.mesh.mesh.stage    = stage;
                        desc.mesh.mesh.bytecode = (const char*) heapCopy;
                        desc.mesh.mesh.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_TASK:
                        desc.pipelineType       = RZ_GFX_PIPELINE_TYPE_GRAPHICS;
                        desc.mesh.task.stage    = stage;
                        desc.mesh.task.bytecode = (const char*) heapCopy;
                        desc.mesh.task.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_RAY_GEN:
                        desc.pipelineType             = RZ_GFX_PIPELINE_TYPE_RAYTRACING;
                        desc.raytracing.rgen.stage    = stage;
                        desc.raytracing.rgen.bytecode = (const char*) heapCopy;
                        desc.raytracing.rgen.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_RAY_MISS:
                        desc.pipelineType             = RZ_GFX_PIPELINE_TYPE_RAYTRACING;
                        desc.raytracing.miss.stage    = stage;
                        desc.raytracing.miss.bytecode = (const char*) heapCopy;
                        desc.raytracing.miss.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_RAY_CLOSEST_HIT:
                        desc.pipelineType             = RZ_GFX_PIPELINE_TYPE_RAYTRACING;
                        desc.raytracing.chit.stage    = stage;
                        desc.raytracing.chit.bytecode = (const char*) heapCopy;
                        desc.raytracing.chit.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_RAY_ANY_HIT:
                        desc.pipelineType             = RZ_GFX_PIPELINE_TYPE_RAYTRACING;
                        desc.raytracing.ahit.stage    = stage;
                        desc.raytracing.ahit.bytecode = (const char*) heapCopy;
                        desc.raytracing.ahit.size     = bytecodeSize;
                        break;

                    case RZ_GFX_SHADER_STAGE_RAY_CALLABLE:
                        desc.pipelineType                 = RZ_GFX_PIPELINE_TYPE_RAYTRACING;
                        desc.raytracing.callable.stage    = stage;
                        desc.raytracing.callable.bytecode = (const char*) heapCopy;
                        desc.raytracing.callable.size     = bytecodeSize;
                        break;

                    default:
                        RAZIX_CORE_WARN("Unknown or unsupported shader stage: {0}", stage);
                        break;
                }
                // bytecode will be freed by RHI, it's a promise we hope RHI keeps upon
            }

            return desc;
        }

#if 0

        static std::unordered_map<std::string, Razix::Gfx::CullMode> CullModeStringMap = {
            {"Back", Razix::Gfx::CullMode::Back},
            {"Front", Razix::Gfx::CullMode::Front},
            {"FrontBack", Razix::Gfx::CullMode::FrontBack},
            {"None", Razix::Gfx::CullMode::None},
        };

        static std::unordered_map<std::string, Razix::Gfx::TextureFormat> TextureFormatStringMap = {
            {"R8", Razix::Gfx::TextureFormat::R8},
            {"R32_INT", Razix::Gfx::TextureFormat::R32_INT},
            {"R32_UINT", Razix::Gfx::TextureFormat::R32_UINT},
            {"R32F", Razix::Gfx::TextureFormat::R32F},
            {"RG8", Razix::Gfx::TextureFormat::RG8},
            {"RGB8", Razix::Gfx::TextureFormat::RGB8},
            {"RGBA8", Razix::Gfx::TextureFormat::RGBA8},
            {"RGB16", Razix::Gfx::TextureFormat::RGB16},
            {"RGBA16", Razix::Gfx::TextureFormat::RGBA16},
            {"RGB32", Razix::Gfx::TextureFormat::RGB32},
            {"RGBA32", Razix::Gfx::TextureFormat::RGBA32},
            {"RGBA32F", Razix::Gfx::TextureFormat::RGBA32F},
            {"RGB", Razix::Gfx::TextureFormat::RGB},
            {"RGBA", Razix::Gfx::TextureFormat::RGBA},
            {"DEPTH16_UNORM", Razix::Gfx::TextureFormat::DEPTH16_UNORM},
            {"DEPTH32F", Razix::Gfx::TextureFormat::DEPTH32F},
            {"STENCIL", Razix::Gfx::TextureFormat::STENCIL},
            {"DEPTH_STENCIL", Razix::Gfx::TextureFormat::DEPTH_STENCIL},
            {"SCREEN", Razix::Gfx::TextureFormat::SCREEN},
            {"BGRA8_UNORM", Razix::Gfx::TextureFormat::BGRA8_UNORM},
            {"R11G11B10A2_UINT", Razix::Gfx::TextureFormat::R11G11B10A2_UINT},
            {"R11G11B10A2_SFLOAT", Razix::Gfx::TextureFormat::R11G11B10A2_SFLOAT}};

        static std::unordered_map<std::string, Razix::Gfx::Wrapping> TextureWrapModeStringMap = {
            {"REPEAT", Razix::Gfx::Wrapping::kRepeat},
            {"MIRRORED_REPEAT", Razix::Gfx::Wrapping::kMirroredRepeat},
            {"CLAMP_TO_EDGE", Razix::Gfx::Wrapping::kClampToEdge},
            {"CLAMP_TO_BORDER", Razix::Gfx::Wrapping::kClampToBorder}};

        static std::unordered_map<std::string, Razix::Gfx::Filtering::Mode> TextureFiletingModeStringMap = {
            {"LINEAR", Razix::Gfx::Filtering::Mode::kFilterModeLinear},
            {"NEAREST", Razix::Gfx::Filtering::Mode::kFilterModeNearest}};

        static std::unordered_map<std::string, Razix::Gfx::BlendFactor>
            BlendFactorStringMap = {
                {"Zero", Razix::Gfx::BlendFactor::Zero},
                {"One", Razix::Gfx::BlendFactor::One},
                {"SrcColor", Razix::Gfx::BlendFactor::SrcColor},
                {"OneMinusSrcColor", Razix::Gfx::BlendFactor::OneMinusSrcColor},
                {"DstColor", Razix::Gfx::BlendFactor::DstColor},
                {"OneMinusDstColor", Razix::Gfx::BlendFactor::OneMinusDstColor},
                {"SrcAlpha", Razix::Gfx::BlendFactor::SrcAlpha},
                {"OneMinusSrcAlpha", Razix::Gfx::BlendFactor::OneMinusSrcAlpha},
                {"DstAlpha", Razix::Gfx::BlendFactor::DstAlpha},
                {"OneMinusDstAlpha", Razix::Gfx::BlendFactor::OneMinusDstAlpha},
                {"ConstantColor", Razix::Gfx::BlendFactor::ConstantColor},
                {"OneMinusConstantColor", Razix::Gfx::BlendFactor::OneMinusConstantColor},
                {"ConstantAlpha", Razix::Gfx::BlendFactor::ConstantAlpha},
                {"OneMinusConstantAlpha", Razix::Gfx::BlendFactor::OneMinusConstantAlpha},
                {"SrcAlphaSaturate", Razix::Gfx::BlendFactor::SrcAlphaSaturate}};

        static std::unordered_map<std::string, Razix::Gfx::BufferUsage> BufferUsageStringMap = {
            {"Static", Razix::Gfx::BufferUsage::Static},
            {"PersistentStream", Razix::Gfx::BufferUsage::PersistentStream},
            {"Staging", Razix::Gfx::BufferUsage::Staging},
            {"ReadBack", Razix::Gfx::BufferUsage::ReadBack},
            {"IndirectDrawArgs", Razix::Gfx::BufferUsage::IndirectDrawArgs}};

        static std::unordered_map<std::string, Razix::Gfx::rz_gfx_shader_stage> rz_gfx_shader_stageStringMap = {
            {"NONE", Razix::Gfx::rz_gfx_shader_stage::kNone},
            {"VERTEX", Razix::Gfx::rz_gfx_shader_stage::kVertex},
            {"PIXEL", Razix::Gfx::rz_gfx_shader_stage::kPixel},
            {"COMPUTE", Razix::Gfx::rz_gfx_shader_stage::kCompute},
            {"GEOMETRY", Razix::Gfx::rz_gfx_shader_stage::kGeometry},
            {"TCS", Razix::Gfx::rz_gfx_shader_stage::kTesselationControl},
            {"TES", Razix::Gfx::rz_gfx_shader_stage::kTesselationEvaluation}};

        static std::unordered_map<std::string, Gfx::TextureType> StringTextureTypeMap = {

            {"Texture_1D", Gfx::TextureType::k1D},
            {"Texture_2D", Gfx::TextureType::k2D},
            {"Texture_2DArray", Gfx::TextureType::k2DArray},
            {"Texture_3D", Gfx::TextureType::k3D},
            {"Texture_Depth", Gfx::TextureType::kDepth},
            {"Texture_CubeMap", Gfx::TextureType::kCubeMap},
            {"Texture_CubeMapArray", Gfx::TextureType::kCubeMapArray},
            {"Texture_SwapchainImage", Gfx::TextureType::kSwapchainImage}};

        static std::unordered_map<std::string, Gfx::TextureFormat> StringTextureFormatMap = {
            {"RGBA32F", Gfx::TextureFormat::RGBA32F},
            {"R8", Gfx::TextureFormat::R8},
            {"R32_INT", Gfx::TextureFormat::R32_INT},
            {"R32_UINT", Gfx::TextureFormat::R32_UINT},
            {"R32F", Gfx::TextureFormat::R32F},
            {"RG8", Gfx::TextureFormat::RG8},
            {"RG16F", Gfx::TextureFormat::RG16F},
            {"RGB8", Gfx::TextureFormat::RGB8},
            {"RGBA8", Gfx::TextureFormat::RGBA8},
            {"RGB16", Gfx::TextureFormat::RGB16},
            {"RGBA16", Gfx::TextureFormat::RGBA16},
            {"RGBA16F", Gfx::TextureFormat::RGBA16F},
            {"RGB32", Gfx::TextureFormat::RGB32},
            {"RGBA32", Gfx::TextureFormat::RGBA32},
            {"RGB", Gfx::TextureFormat::RGB},
            {"RGBA", Gfx::TextureFormat::RGBA},
            {"DEPTH16_UNORM", Gfx::TextureFormat::DEPTH16_UNORM},
            {"DEPTH32F", Gfx::TextureFormat::DEPTH32F},
            {"STENCIL", Gfx::TextureFormat::STENCIL},
            {"DEPTH_STENCIL", Gfx::TextureFormat::DEPTH_STENCIL},
            {"SCREEN", Gfx::TextureFormat::SCREEN},
            {"BGRA8_UNORM", Gfx::TextureFormat::BGRA8_UNORM},
            {"R11G11B10A2_UINT", Gfx::TextureFormat::R11G11B10A2_UINT},
            {"R11G11B10A2_SFLOAT", Gfx::TextureFormat::R11G11B10A2_SFLOAT},
            {"NONE", Gfx::TextureFormat::NONE}};

        static std::unordered_map<std::string, Razix::Gfx::CompareOp>
            CompareOpStringMap = {
                {"Never", Razix::Gfx::CompareOp::Never},
                {"Less", Razix::Gfx::CompareOp::Less},
                {"Equal", Razix::Gfx::CompareOp::Equal},
                {"LessOrEqual", Razix::Gfx::CompareOp::LessOrEqual},
                {"Greater", Razix::Gfx::CompareOp::Greater},
                {"NotEqual", Razix::Gfx::CompareOp::NotEqual},
                {"GreaterOrEqual", Razix::Gfx::CompareOp::GreaterOrEqual},
                {"Always", Razix::Gfx::CompareOp::Always}};

        static std::unordered_map<std::string, Razix::Gfx::BlendOp> BlendOpStringMap = {
            {"Add", Razix::Gfx::BlendOp::Add},
            {"Subtract", Razix::Gfx::BlendOp::Subtract},
            {"ReverseSubtract", Razix::Gfx::BlendOp::ReverseSubtract},
            {"Min", Razix::Gfx::BlendOp::Min},
            {"Max", Razix::Gfx::BlendOp::Max}};

        static std::unordered_map<std::string, Razix::Gfx::DrawType> DrawTypeStringMap = {
            {"Triangle", Razix::Gfx::DrawType::Triangle},
            {"Line", Razix::Gfx::DrawType::Line},
            {"Point", Razix::Gfx::DrawType::Point}};

        static std::unordered_map<std::string, Razix::Gfx::PolygonMode> PolygonModeStringMap = {
            {"Fill", Razix::Gfx::PolygonMode::Fill},
            {"Line", Razix::Gfx::PolygonMode::Line},
            {"Point", Razix::Gfx::PolygonMode::Point}};

        // Modified from Source: https://github.com/skaarj1989/SupernovaEngine/blob/57f0fba69b36de45255971080ea082bea8219cbb/modules/Renderer/WorldRenderer/src/FrameGraphResourceAccess.cpp (MIT License)

        /**
         * BindingLocation : 9 bits
         * 
         * encoding format:
         * set, binding each of them max take up to 32 each so even 8 bits for each is still a lot
         * cause shaders won't have more than 32 sets/bindings at one point
         * 
         * | 4 bits  | 5 bits  |
         * | 0...16  | 0...16  |
         * |   set   | binding |
         */

        /**
         * DescriptorBindingInfo : 29 bits
         * 
         * encoding format:
         * count is array elements of max 1024 which might be less but we're already packing this up
         * 
         * 
         * | 3 bits | 5 bits | 9 bits   | 12        |
         * | 0...7  | 0...5  | 0...8 x2 | 0...4096  |
         * | type   | stage  | binding  | count     |
         * 
         */

        /**
         * AttachmentInfo : 22 bits
         * 
         * encoding format:
         * Even a 16k texture (15360 × 8640) would have maximum mips of 14 so 4 bits are more than enough to represent mips
         * As for layers let's say even if we have texture arrays abnormally large let's say they have 256 or at max 1024 we need 8...10 bits at max
         * 
         * | 1 bits |  3 bits      | 4 bits    | 4 bits   | 10 bits   |
         * | 0...1  |  0...8       | 0...15    | 0...16   | 0...1024  |
         * | clear  | clear color  | binding   | mips     | layer     |
         * 
         */

        constexpr auto kBindingLocationBits  = 9;
        constexpr auto kSetIndexBits         = 4;
        constexpr auto kBindingIndexBits     = 5;
        constexpr auto kSetIndexOffset       = 0;
        constexpr auto kBindingIndexOffset   = kSetIndexOffset + kSetIndexBits;
        constexpr auto kTypeBits             = 3;
        constexpr auto kStageBits            = 5;
        constexpr auto kCountBits            = 12;
        constexpr auto kTypeOffset           = 0;
        constexpr auto kStageOffset          = kTypeOffset + kTypeBits;
        constexpr auto kBindingLocOffset     = kStageOffset + kStageBits;
        constexpr auto kCountOffset          = kBindingLocOffset + kBindingLocationBits;
        constexpr auto kClearBits            = 1;
        constexpr auto kClearColorBits       = 3;
        constexpr auto kBindingRTBits        = 4;
        constexpr auto kMipsBits             = 4;
        constexpr auto kLayerBits            = 10;
        constexpr auto kClearBitsOffset      = 0;
        constexpr auto kClearColorBitsOffset = kClearBitsOffset + kClearBits;
        constexpr auto kBindingRTBitsOffset  = kClearColorBitsOffset + kClearColorBits;
        constexpr auto kMipsBitsOffset       = kBindingRTBits + kBindingRTBitsOffset;
        constexpr auto kLayerBitsOffset      = kMipsBits + kMipsBitsOffset;

        RAZIX_NO_DISCARD u32 EncodeBindingLocation(BindingLocation info)
        {
            uint32_t bits{0};
            bits = BIT_INSERT(bits, info.set, kSetIndexOffset, kSetIndexBits);
            bits = BIT_INSERT(bits, info.binding, kBindingIndexOffset, kBindingIndexBits);

            return bits;
        }

        Razix::Gfx::BindingLocation DecodeBindingLocation(u32 bits)
        {
            BindingLocation info{};
            info.set     = BIT_EXTRACT(bits, kSetIndexOffset, kSetIndexBits);
            info.binding = BIT_EXTRACT(bits, kBindingIndexOffset, kBindingIndexBits);

            return info;
        }

        RAZIX_NO_DISCARD u32 EncodeDescriptorBindingInfo(DescriptorBindingInfo info)
        {
            uint32_t bits{0};
            bits = BIT_INSERT(bits, (u32) info.type, kTypeOffset, kTypeBits);
            bits = BIT_INSERT(bits, (u32) info.stage, kStageOffset, kStageBits);
            bits = BIT_INSERT(bits, (u32) EncodeBindingLocation(info.location), kBindingLocOffset, kBindingLocationBits);
            bits = BIT_INSERT(bits, (u32) info.count, kCountOffset, kCountBits);

            return bits;
        }

        Razix::Gfx::DescriptorBindingInfo DecodeDescriptorBindingInfo(u32 bits)
        {
            DescriptorBindingInfo info{};
            info.type     = (DescriptorType) BIT_EXTRACT(bits, kTypeOffset, kTypeBits);
            info.stage    = (rz_gfx_shader_stage) BIT_EXTRACT(bits, kStageOffset, kStageBits);
            info.location = DecodeBindingLocation(BIT_EXTRACT(bits, kBindingLocOffset, kBindingLocationBits));
            info.count    = BIT_EXTRACT(bits, kCountOffset, kCountBits);

            return info;
        }

        RAZIX_NO_DISCARD u32 EncodeAttachmentInfo(RenderTargetAttachmentInfo info)
        {
            uint32_t bits{0};
            bits = BIT_INSERT(bits, (u32) info.clear, kClearBitsOffset, kClearBits);
            bits = BIT_INSERT(bits, (u32) info.clearColor, kClearColorBitsOffset, kClearColorBits);
            bits = BIT_INSERT(bits, info.bindingIdx, kBindingRTBitsOffset, kBindingRTBits);
            bits = BIT_INSERT(bits, info.mip, kMipsBitsOffset, kMipsBits);
            bits = BIT_INSERT(bits, info.layer, kLayerBitsOffset, kLayerBits);

            return bits;
        }

        RenderTargetAttachmentInfo DecodeAttachmentInfo(u32 bits)
        {
            RenderTargetAttachmentInfo info{};
            info.clear      = BIT_EXTRACT(bits, kClearBitsOffset, kClearBits);
            info.clearColor = (ClearColorPresets) BIT_EXTRACT(bits, kClearColorBitsOffset, kClearColorBits);
            info.bindingIdx = BIT_EXTRACT(bits, kBindingRTBitsOffset, kBindingRTBits);
            info.mip        = BIT_EXTRACT(bits, kMipsBitsOffset, kMipsBits);
            info.layer      = BIT_EXTRACT(bits, kLayerBitsOffset, kLayerBits);

            return info;
        }

        //-----------------------------------------------------------------------------------

        float4 ClearColorFromPreset(ClearColorPresets preset)
        {
            switch (preset) {
                case Razix::Gfx::ClearColorPresets::OpaqueBlack:
                    return float4(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
                case Razix::Gfx::ClearColorPresets::OpaqueWhite:
                    return float4(1.0f, 1.0f, 1.0f, 1.0f);
                    break;
                case Razix::Gfx::ClearColorPresets::TransparentBlack:
                    return float4(0.0f, 0.0f, 0.0f, 0.0f);
                    break;
                case Razix::Gfx::ClearColorPresets::TransparentWhite:
                    return float4(1.0f, 1.0f, 1.0f, 0.0f);
                    break;
                case Razix::Gfx::ClearColorPresets::Pink:
                    return float4(1.0f, 0.0f, 1.0f, 1.0f);
                    break;
                case Razix::Gfx::ClearColorPresets::DepthZeroToOne:
                    return float4(0.0f, 1.0f, 0.0f, 0.0f);
                    break;
                case Razix::Gfx::ClearColorPresets::DepthOneToZero:
                    return float4(1.0f, 0.0f, 0.0f, 0.0f);
                    break;
                default:
                    return float4(0.0f, 0.0f, 0.0f, 1.0f);
                    break;
            }
        }

        //-----------------------------------------------------------------------------------

        std::string RZTextureDesc::FormatToString(const Gfx::TextureFormat format)
        {
            switch (format) {
                case Razix::Gfx::TextureFormat::R8:
                    return "R8";
                    break;
                case Razix::Gfx::TextureFormat::R32_INT:
                    return "R32_INT";
                    break;
                case Razix::Gfx::TextureFormat::R32_UINT:
                    return "R32_UINT";
                    break;
                case Razix::Gfx::TextureFormat::R32F:
                    return "R32F";
                    break;
                case Razix::Gfx::TextureFormat::RG8:
                    return "RG8";
                    break;
                case Razix::Gfx::TextureFormat::RG16F:
                    return "RG16F";
                    break;
                case Razix::Gfx::TextureFormat::RGB8:
                    return "RGB8";
                    break;
                case Razix::Gfx::TextureFormat::RGBA8:
                    return "RGBA8";
                    break;
                case Razix::Gfx::TextureFormat::RGB16:
                    return "RGB16";
                    break;
                case Razix::Gfx::TextureFormat::RGBA16:
                    return "RGBA16";
                    break;
                case Razix::Gfx::TextureFormat::RGBA16F:
                    return "RGBA16F";
                    break;
                case Razix::Gfx::TextureFormat::RGB32:
                    return "RGB32";
                    break;
                case Razix::Gfx::TextureFormat::RGBA32:
                    return "RGBA32";
                    break;
                case Razix::Gfx::TextureFormat::RGBA32F:
                    return "RGBA32F";
                    break;
                case Razix::Gfx::TextureFormat::RGB:
                    return "RGB";
                    break;
                case Razix::Gfx::TextureFormat::RGBA:
                    return "RGBA";
                    break;
                case Razix::Gfx::TextureFormat::DEPTH16_UNORM:
                    return "DEPTH";
                    break;
                case Razix::Gfx::TextureFormat::STENCIL:
                    return "STENCIL";
                    break;
                case Razix::Gfx::TextureFormat::DEPTH_STENCIL:
                    return "DEPTH_STENCIL";
                    break;
                case Razix::Gfx::TextureFormat::SCREEN:
                    return "Presentation";
                    break;
                case Razix::Gfx::TextureFormat::BGRA8_UNORM:
                    return "BGRA8_UNORM";
                    break;
                default:
                    return "BGRA8_UNORM";
                    break;
                    break;
            }
            return "BGRA8_UNORM";
        }

        std::string RZTextureDesc::TypeToString(TextureType type)
        {
            switch (type) {
                case Razix::Gfx::TextureType::k2D:
                    return "2D";
                    break;
                case Razix::Gfx::TextureType::k3D:
                    return "3D";
                    break;
                case Razix::Gfx::TextureType::kCubeMap:
                    return "CubeMap";
                    break;
                case Razix::Gfx::TextureType::kDepth:
                    return "Depth";
                    break;
                case Razix::Gfx::TextureType::kSwapchainImage:
                    return "Swapchain Image";
                    break;
                case TextureType::k1D:
                    return "1D";
                    break;
                case TextureType::k2DArray:
                    return "2D Array";
                    break;
                case TextureType::kCubeMapArray:
                    return "CubeMap Array";
                    break;
                default:
                    return "NONE";
                    break;
            }
            return "NONE";
        }

        //-----------------------------------------------------------------------------------

        Razix::Gfx::TextureFormat RZTextureDesc::StringToFormat(const std::string& str)
        {
            RAZIX_ASSERT((StringTextureFormatMap.size() == (u32) TextureFormat::COUNT), "TextureFormat string enum map has missing values");
            return StringTextureFormatMap[str];
        }

        Razix::Gfx::TextureType RZTextureDesc::StringToType(const std::string& str)
        {
            return StringTextureTypeMap[str];
        }

        Razix::Gfx::CompareOp StringToCompareOp(const std::string& str)
        {
            return CompareOpStringMap[str];
        }

        Razix::Gfx::BlendOp StringToBlendOp(const std::string& str)
        {
            return BlendOpStringMap[str];
        }

        Razix::Gfx::DrawType StringToDrawType(const std::string& str)
        {
            return DrawTypeStringMap[str];
        }

        Razix::Gfx::PolygonMode StringToPolygonMode(const std::string& str)
        {
            return PolygonModeStringMap[str];
        }

        Razix::Gfx::CullMode StringToCullMode(const std::string& str)
        {
            return CullModeStringMap[str];
        }

        TextureFormat StringToTextureFormat(const std::string& str)
        {
            return TextureFormatStringMap[str];
        }

        Razix::Gfx::Wrapping StringToWrapping(const std::string& str)
        {
            return TextureWrapModeStringMap[str];
        }

        Razix::Gfx::Filtering::Mode StringToFilteringMode(const std::string& str)
        {
            return TextureFiletingModeStringMap[str];
        }

        Razix::Gfx::BlendFactor StringToBlendFactor(const std::string& str)
        {
            return BlendFactorStringMap[str];
        }

        Razix::Gfx::BufferUsage StringToBufferUsage(const std::string& str)
        {
            return BufferUsageStringMap[str];
        }

        Razix::Gfx::rz_gfx_shader_stage StringTorz_gfx_shader_stage(const std::string& str)
        {
            return rz_gfx_shader_stageStringMap[str];
        }

        //-----------------------------------------------------------------------------------

        u32 BufferLayoutElement::getCount()
        {
            switch (format) {
                case Razix::Gfx::BufferFormat::R8_INT:
                case Razix::Gfx::BufferFormat::R8_UINT:
                case Razix::Gfx::BufferFormat::R32_INT:
                case Razix::Gfx::BufferFormat::R32_UINT:
                case Razix::Gfx::BufferFormat::R32_FLOAT:
                    return 1;
                    break;
                case Razix::Gfx::BufferFormat::R32G32_INT:
                case Razix::Gfx::BufferFormat::R32G32_UINT:
                case Razix::Gfx::BufferFormat::R32G32_FLOAT:
                    return 2;
                    break;
                case Razix::Gfx::BufferFormat::R32G32B32_INT:
                case Razix::Gfx::BufferFormat::R32G32B32_UINT:
                case Razix::Gfx::BufferFormat::R32G32B32_FLOAT:
                    return 3;
                    break;
                case Razix::Gfx::BufferFormat::R32G32B32A32_INT:
                case Razix::Gfx::BufferFormat::R32G32B32A32_UINT:
                case Razix::Gfx::BufferFormat::R32G32B32A32_FLOAT:
                    return 4;
                    break;
                default:
                    return 0;
                    break;
            }
        }

        void RZBufferLayout::pushImpl(const std::string& name, BufferFormat format, u32 size, bool Normalised)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            m_Elements.push_back({name, format, m_Stride, Normalised});
            m_Stride += size;
        }, mo
#endif

    }    // namespace Gfx
}    // namespace Razix
