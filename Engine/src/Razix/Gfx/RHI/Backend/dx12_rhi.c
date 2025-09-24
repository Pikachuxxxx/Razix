#include "dx12_rhi.h"

#include "Razix/Gfx/RHI/RHI.h"

#include <d3d12shader.h>
#include <d3dcompiler.h>

#include <malloc.h>    // for alloca

// TYpe friendly defines
#define DX12Context g_GfxCtx.dx12
#define DX12Device  g_GfxCtx.dx12.device10
#if defined RAZIX_DEBUG
    #define CHECK_HR(x) dx12_util_check_hresult((x), __func__, __FILE__, __LINE__)
    #define TAG_OBJECT(resource, name)                                              \
        if (resource) {                                                             \
            resource->lpVtbl->SetName(resource, dx12_util_string_to_lpcwstr(name)); \
        }
#else
    #define CHECK_HR(x) (x)
    #define TAG_OBJECT(resource, name)
#endif

//---------------------------------------------------------------------------------------------
// Internal types

#ifdef RAZIX_DEBUG
typedef struct HRESULTDescriptionEntry
{
    HRESULT     code;
    const char* description;
} HRESULTDescriptionEntry;

static HRESULTDescriptionEntry hresult_errors[] = {
    {S_OK, "Operation successful"},
    {E_ABORT, "Operation aborted"},
    {E_ACCESSDENIED, "General access denied error"},
    {E_FAIL, "Unspecified failure"},
    {E_HANDLE, "Handle that is not valid"},
    {E_INVALIDARG, "One or more arguments are not valid"},
    {E_NOINTERFACE, "No such interface supported"},
    {E_NOTIMPL, "Not implemented"},
    {E_OUTOFMEMORY, "Failed to allocate necessary memory"},
    {E_POINTER, "Pointer that is not valid"},
    {E_UNEXPECTED, "Unexpected failure"},
};
#endif

//---------------------------------------------------------------------------------------------
// Util functions

#ifdef RAZIX_DEBUG
static const char* dx12_util_hresult_to_string(HRESULT hr)
{
    size_t count = sizeof(hresult_errors) / sizeof(hresult_errors[0]);
    for (size_t i = 0; i < count; ++i) {
        if (hresult_errors[i].code == hr)
            return hresult_errors[i].description;
    }
    return "Unknown HRESULT error";
}

static bool dx12_util_check_hresult(HRESULT hr, const char* func, const char* file, int line)
{
    if (hr != S_OK) {
        const char* desc = dx12_util_hresult_to_string(hr);
        RAZIX_RHI_LOG_ERROR("[D3D12] HRESULT Error :: %s\n -> In function %s (%s:%d)\n", desc, func, file, line);
        return false;
    }
    return true;
}

static LPCWSTR dx12_util_string_to_lpcwstr(const char* input)
{
    if (!input) return NULL;

    int wide_len = MultiByteToWideChar(CP_UTF8, 0, input, -1, NULL, 0);
    if (wide_len == 0) return NULL;

    wchar_t* wide_str = (wchar_t*) malloc(wide_len * sizeof(wchar_t));
    if (!wide_str) return NULL;

    if (MultiByteToWideChar(CP_UTF8, 0, input, -1, wide_str, wide_len) == 0) {
        free(wide_str);
        return NULL;
    }
    return wide_str;
}
#endif

static DXGI_FORMAT dx12_util_rz_gfx_format_to_dxgi_format(rz_gfx_format format)
{
    switch (format) {
        // 8-bit
        case RZ_GFX_FORMAT_R8_UNORM: return DXGI_FORMAT_R8_UNORM;
        case RZ_GFX_FORMAT_R8_UINT: return DXGI_FORMAT_R8_UINT;

        // 16-bit
        case RZ_GFX_FORMAT_R16_UNORM: return DXGI_FORMAT_R16_UNORM;
        case RZ_GFX_FORMAT_R16_FLOAT: return DXGI_FORMAT_R16_FLOAT;
        case RZ_GFX_FORMAT_R16G16_FLOAT: return DXGI_FORMAT_R16G16_FLOAT;
        case RZ_GFX_FORMAT_R16G16_UNORM: return DXGI_FORMAT_R16G16_UNORM;
        case RZ_GFX_FORMAT_R16G16B16A16_UNORM: return DXGI_FORMAT_R16G16B16A16_UNORM;
        case RZ_GFX_FORMAT_R16G16B16A16_FLOAT: return DXGI_FORMAT_R16G16B16A16_FLOAT;

        // 32-bit
        case RZ_GFX_FORMAT_R32_SINT: return DXGI_FORMAT_R32_SINT;
        case RZ_GFX_FORMAT_R32_UINT: return DXGI_FORMAT_R32_UINT;
        case RZ_GFX_FORMAT_R32_FLOAT: return DXGI_FORMAT_R32_FLOAT;
        case RZ_GFX_FORMAT_R32G32_SINT: return DXGI_FORMAT_R32G32_SINT;
        case RZ_GFX_FORMAT_R32G32_UINT: return DXGI_FORMAT_R32G32_UINT;
        case RZ_GFX_FORMAT_R32G32_FLOAT: return DXGI_FORMAT_R32G32_FLOAT;
        case RZ_GFX_FORMAT_R32G32B32_SINT: return DXGI_FORMAT_R32G32B32_SINT;
        case RZ_GFX_FORMAT_R32G32B32_UINT: return DXGI_FORMAT_R32G32B32_UINT;
        case RZ_GFX_FORMAT_R32G32B32_FLOAT: return DXGI_FORMAT_R32G32B32_FLOAT;
        case RZ_GFX_FORMAT_R32G32B32A32_SINT: return DXGI_FORMAT_R32G32B32A32_SINT;
        case RZ_GFX_FORMAT_R32G32B32A32_UINT: return DXGI_FORMAT_R32G32B32A32_UINT;
        case RZ_GFX_FORMAT_R32G32B32A32_FLOAT: return DXGI_FORMAT_R32G32B32A32_FLOAT;

        // Packed
        case RZ_GFX_FORMAT_R11G11B10_FLOAT: return DXGI_FORMAT_R11G11B10_FLOAT;
        case RZ_GFX_FORMAT_R11G11B10_UINT:
            return DXGI_FORMAT_UNKNOWN;    // No DXGI_FORMAT for UINT variant

        // Color formats
        case RZ_GFX_FORMAT_R8G8_UNORM: return DXGI_FORMAT_R8G8_UNORM;
        case RZ_GFX_FORMAT_R8G8B8_UNORM: return DXGI_FORMAT_UNKNOWN;    // No native DXGI 24-bit format
        case RZ_GFX_FORMAT_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case RZ_GFX_FORMAT_R8G8B8A8_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case RZ_GFX_FORMAT_B8G8R8A8_UNORM: return DXGI_FORMAT_B8G8R8A8_UNORM;
        case RZ_GFX_FORMAT_B8G8R8A8_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

        // Legacy/placeholder
        case RZ_GFX_FORMAT_RGB8_UNORM: return DXGI_FORMAT_UNKNOWN;
        case RZ_GFX_FORMAT_RGB16_UNORM: return DXGI_FORMAT_UNKNOWN;
        case RZ_GFX_FORMAT_RGB32_UINT: return DXGI_FORMAT_UNKNOWN;
        case RZ_GFX_FORMAT_RGBA: return DXGI_FORMAT_UNKNOWN;
        case RZ_GFX_FORMAT_RGB: return DXGI_FORMAT_UNKNOWN;

        // Depth-stencil
        case RZ_GFX_FORMAT_D16_UNORM: return DXGI_FORMAT_D16_UNORM;
        case RZ_GFX_FORMAT_D24_UNORM_S8_UINT: return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case RZ_GFX_FORMAT_D32_FLOAT: return DXGI_FORMAT_D32_FLOAT;
        case RZ_GFX_FORMAT_D32_FLOAT_S8X24_UINT: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        case RZ_GFX_FORMAT_STENCIL8: return DXGI_FORMAT_D24_UNORM_S8_UINT;

        // Swapchain pseudo format
        case RZ_GFX_FORMAT_SCREEN: return RAZIX_SWAPCHAIN_FORMAT_DX12;

        // Block compression
        case RZ_GFX_FORMAT_BC1_RGBA_UNORM: return DXGI_FORMAT_BC1_UNORM;
        case RZ_GFX_FORMAT_BC3_RGBA_UNORM: return DXGI_FORMAT_BC3_UNORM;
        case RZ_GFX_FORMAT_BC6_UNORM: return DXGI_FORMAT_BC6H_UF16;
        case RZ_GFX_FORMAT_BC7_UNORM: return DXGI_FORMAT_BC7_UNORM;
        case RZ_GFX_FORMAT_BC7_SRGB: return DXGI_FORMAT_BC7_UNORM_SRGB;

        case RZ_GFX_FORMAT_UNDEFINED:
        default: return DXGI_FORMAT_UNKNOWN;
    }
}

static D3D12_COMMAND_LIST_TYPE dx12_util_rz_cmdpool_to_cmd_list_type(rz_gfx_cmdpool_type type)
{
    switch (type) {
        case RZ_GFX_CMDPOOL_TYPE_GRAPHICS: return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case RZ_GFX_CMDPOOL_TYPE_COMPUTE: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
        case RZ_GFX_CMDPOOL_TYPE_TRANSFER: return D3D12_COMMAND_LIST_TYPE_COPY;
        default: return D3D12_COMMAND_LIST_TYPE_DIRECT;    // Fallback to graphics
    }
}

static const D3D12_RESOURCE_STATES d3d12_resource_state_map[RZ_GFX_RESOURCE_STATE_COUNT] = {
    D3D12_RESOURCE_STATE_COMMON,                                                                    // UNDEFINED
    D3D12_RESOURCE_STATE_COMMON,                                                                    // COMMON
    D3D12_RESOURCE_STATE_GENERIC_READ,                                                              // GENERIC_READ
    D3D12_RESOURCE_STATE_RENDER_TARGET,                                                             // RENDER_TARGET
    D3D12_RESOURCE_STATE_DEPTH_WRITE,                                                               // DEPTH_WRITE
    D3D12_RESOURCE_STATE_DEPTH_READ,                                                                // DEPTH_READ
    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,    // SHADER_READ
    D3D12_RESOURCE_STATE_UNORDERED_ACCESS,                                                          // UNORDERED_ACCESSO
    D3D12_RESOURCE_STATE_COPY_SOURCE,                                                               // COPY_SRC
    D3D12_RESOURCE_STATE_COPY_DEST,                                                                 // COPY_DST
    D3D12_RESOURCE_STATE_PRESENT,                                                                   // PRESENT
    D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,                                                // VERTEX_BUFFER
    D3D12_RESOURCE_STATE_INDEX_BUFFER,                                                              // INDEX_BUFFER
    D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,                                                // CONSTANT_BUFFER
    D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,                                                         // INDIRECT_ARGUMENT
    D3D12_RESOURCE_STATE_RESOLVE_SOURCE,                                                            // RESOLVE_SRC
    D3D12_RESOURCE_STATE_RESOLVE_DEST,                                                              // RESOLVE_DST
    D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,                                         // RAYTRACING_ACCELERATION_STRUCTURE
    D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE,                                                       // SHADING_RATE_SOURCE
    D3D12_RESOURCE_STATE_VIDEO_DECODE_READ,                                                         // VIDEO_DECODE_READ
    D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE,                                                        // VIDEO_DECODE_WRITE
};

static D3D12_RESOURCE_STATES dx12_util_res_state_translate(rz_gfx_resource_state state)
{
    if (state >= RZ_GFX_RESOURCE_STATE_COUNT || state == RZ_GFX_RESOURCE_STATE_UNDEFINED) {
        RAZIX_RHI_LOG_ERROR("Invalid resource state %d", state);
        return D3D12_RESOURCE_STATE_COMMON;
    }

    return d3d12_resource_state_map[state];
}

static D3D12_DESCRIPTOR_RANGE_TYPE dx12_util_descriptor_type_to_range_type(rz_gfx_descriptor_type type)
{
    switch (type) {
        case RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER:
            return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

        case RZ_GFX_DESCRIPTOR_TYPE_TEXTURE:
        case RZ_GFX_DESCRIPTOR_TYPE_STRUCTURED:
        case RZ_GFX_DESCRIPTOR_TYPE_BYTE_ADDRESS:
        case RZ_GFX_DESCRIPTOR_TYPE_RT_ACCELERATION_STRUCTURE:
            // Byte-address and structured-read are SRVs; RT AS is bound as SRV
            return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

        case RZ_GFX_DESCRIPTOR_TYPE_RW_TEXTURE:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_TYPED:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_BYTE_ADDRESS:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED_COUNTER:
        case RZ_GFX_DESCRIPTOR_TYPE_APPEND_STRUCTURED:
        case RZ_GFX_DESCRIPTOR_TYPE_CONSUME_STRUCTURED:
            return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;

        case RZ_GFX_DESCRIPTOR_TYPE_SAMPLER:
            return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;

        case RZ_GFX_DESCRIPTOR_TYPE_PUSH_CONSTANT:
            // D3D12 doesn't have a descriptor range type for push constants.
            // These are typically bound via root constants instead of descriptor tables.
            RAZIX_RHI_ASSERT(false, "Push constants must be bound via root constants, not descriptor ranges");
            return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

        case RZ_GFX_DESCRIPTOR_TYPE_IMAGE_SAMPLER_COMBINED:
            // Vulkan-only combined sampler+texture; youll need two separate ranges in D3D12.
            RAZIX_RHI_ASSERT(false, "Combined image/sampler isn't supported directly in DX12");
            return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

        case RZ_GFX_DESCRIPTOR_TYPE_NONE:
        case RZ_GFX_DESCRIPTOR_TYPE_COUNT:
        default:
            RAZIX_RHI_ASSERT(false, "Unknown or invalid descriptor type");
            return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    }
}

static D3D12_SHADER_BYTECODE dx12_util_shader_bytecode_to_d3d12_shader(const rz_gfx_shader_stage_blob* blob)
{
    D3D12_SHADER_BYTECODE shaderBytecode = {0};
    if (blob && blob->bytecode && blob->size > 0) {
        shaderBytecode.pShaderBytecode = blob->bytecode;
        shaderBytecode.BytecodeLength  = blob->size;
    } else if (blob && blob->size > 0) {
        RAZIX_RHI_LOG_ERROR("Invalid shader bytecode provided");
    }
    return shaderBytecode;
}

static D3D12_PRIMITIVE_TOPOLOGY dx12_util_draw_type_to_topology(rz_gfx_draw_type drawType)
{
    switch (drawType) {
        case RZ_GFX_DRAW_TYPE_POINT: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        case RZ_GFX_DRAW_TYPE_LINE: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case RZ_GFX_DRAW_TYPE_LINE_STRIP: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case RZ_GFX_DRAW_TYPE_TRIANGLE: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        case RZ_GFX_DRAW_TYPE_TRIANGLE_STRIP: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        default:
            RAZIX_RHI_LOG_ERROR("Unknown draw data type %d", drawType);
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    }
}

static D3D_PRIMITIVE_TOPOLOGY dx12_util_draw_type_to_d3dtopology(rz_gfx_draw_type drawType)
{
    switch (drawType) {
        case RZ_GFX_DRAW_TYPE_POINT: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        case RZ_GFX_DRAW_TYPE_LINE: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        case RZ_GFX_DRAW_TYPE_LINE_STRIP: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case RZ_GFX_DRAW_TYPE_TRIANGLE: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        case RZ_GFX_DRAW_TYPE_TRIANGLE_STRIP: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        default:
            RAZIX_RHI_LOG_ERROR("Unknown draw data type %d", drawType);
            return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }
}

static D3D12_FILL_MODE dx12_util_polygon_mode_to_fill_mode(rz_gfx_polygon_mode_type polygonMode)
{
    switch (polygonMode) {
        case RZ_GFX_POLYGON_MODE_TYPE_SOLID: return D3D12_FILL_MODE_SOLID;
        case RZ_GFX_POLYGON_MODE_TYPE_WIREFRAME: return D3D12_FILL_MODE_WIREFRAME;
        case RZ_GFX_POLYGON_MODE_TYPE_POINT: return D3D12_FILL_MODE_SOLID;
        default:
            RAZIX_RHI_LOG_ERROR("Unknown polygon mode %d", polygonMode);
            return D3D12_FILL_MODE_SOLID;
    }
}

static D3D12_CULL_MODE dx12_util_cull_mode_translate(rz_gfx_cull_mode_type cullmode)
{
    switch (cullmode) {
        case RZ_GFX_CULL_MODE_TYPE_NONE: return D3D12_CULL_MODE_NONE;
        case RZ_GFX_CULL_MODE_TYPE_FRONT: return D3D12_CULL_MODE_FRONT;
        case RZ_GFX_CULL_MODE_TYPE_BACK: return D3D12_CULL_MODE_BACK;
        default:
            RAZIX_RHI_LOG_ERROR("Unknown cull mode %d", cullmode);
            return D3D12_CULL_MODE_NONE;
    }
}

static D3D12_BLEND dx12_util_blend_factor(rz_gfx_blend_factor_type factor)
{
    switch (factor) {
        case RZ_GFX_BLEND_FACTOR_TYPE_ZERO: return D3D12_BLEND_ZERO;
        case RZ_GFX_BLEND_FACTOR_TYPE_ONE: return D3D12_BLEND_ONE;
        case RZ_GFX_BLEND_FACTOR_TYPE_SRC_COLOR: return D3D12_BLEND_SRC_COLOR;
        case RZ_GFX_BLEND_FACTOR_TYPE_ONE_MINUS_SRC_COLOR: return D3D12_BLEND_INV_SRC_COLOR;
        case RZ_GFX_BLEND_FACTOR_TYPE_DST_COLOR: return D3D12_BLEND_DEST_COLOR;
        case RZ_GFX_BLEND_FACTOR_TYPE_ONE_MINUS_DST_COLOR: return D3D12_BLEND_INV_DEST_COLOR;
        case RZ_GFX_BLEND_FACTOR_TYPE_SRC_ALPHA: return D3D12_BLEND_SRC_ALPHA;
        case RZ_GFX_BLEND_FACTOR_TYPE_ONE_MINUS_SRC_ALPHA: return D3D12_BLEND_INV_SRC_ALPHA;
        case RZ_GFX_BLEND_FACTOR_TYPE_DST_ALPHA: return D3D12_BLEND_DEST_ALPHA;
        case RZ_GFX_BLEND_FACTOR_TYPE_ONE_MINUS_DST_ALPHA: return D3D12_BLEND_INV_DEST_ALPHA;
        case RZ_GFX_BLEND_FACTOR_TYPE_CONSTANT_COLOR: return D3D12_BLEND_BLEND_FACTOR;
        case RZ_GFX_BLEND_FACTOR_TYPE_ONE_MINUS_CONSTANT_COLOR: return D3D12_BLEND_INV_BLEND_FACTOR;
        case RZ_GFX_BLEND_FACTOR_TYPE_CONSTANT_ALPHA: return D3D12_BLEND_BLEND_FACTOR;
        case RZ_GFX_BLEND_FACTOR_TYPE_ONE_MINUS_CONSTANT_ALPHA: return D3D12_BLEND_INV_BLEND_FACTOR;
        case RZ_GFX_BLEND_FACTOR_TYPE_SRC_ALPHA_SATURATE: return D3D12_BLEND_SRC_ALPHA_SAT;
        default: return D3D12_BLEND_ONE;
    }
}

static D3D12_BLEND_OP dx12_util_blend_op(rz_gfx_blend_op_type op)
{
    switch (op) {
        case RZ_GFX_BLEND_OP_TYPE_ADD: return D3D12_BLEND_OP_ADD;
        case RZ_GFX_BLEND_OP_TYPE_SUBTRACT: return D3D12_BLEND_OP_SUBTRACT;
        case RZ_GFX_BLEND_OP_TYPE_REVERSE_SUBTRACT: return D3D12_BLEND_OP_REV_SUBTRACT;
        case RZ_GFX_BLEND_OP_TYPE_MIN: return D3D12_BLEND_OP_MIN;
        case RZ_GFX_BLEND_OP_TYPE_MAX: return D3D12_BLEND_OP_MAX;
        default: return D3D12_BLEND_OP_ADD;
    }
}

static D3D12_COMPARISON_FUNC dx12_util_compare_func(rz_gfx_compare_op_type func)
{
    switch (func) {
        case RZ_GFX_COMPARE_OP_TYPE_NEVER: return D3D12_COMPARISON_FUNC_NEVER;
        case RZ_GFX_COMPARE_OP_TYPE_LESS: return D3D12_COMPARISON_FUNC_LESS;
        case RZ_GFX_COMPARE_OP_TYPE_EQUAL: return D3D12_COMPARISON_FUNC_EQUAL;
        case RZ_GFX_COMPARE_OP_TYPE_LESS_OR_EQUAL: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case RZ_GFX_COMPARE_OP_TYPE_GREATER: return D3D12_COMPARISON_FUNC_GREATER;
        case RZ_GFX_COMPARE_OP_TYPE_NOT_EQUAL: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        case RZ_GFX_COMPARE_OP_TYPE_GREATER_OR_EQUAL: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        case RZ_GFX_COMPARE_OP_TYPE_ALWAYS: return D3D12_COMPARISON_FUNC_ALWAYS;
        default: return D3D12_COMPARISON_FUNC_ALWAYS;
    }
}

static D3D12_RENDER_TARGET_BLEND_DESC dx12_util_blend_preset(rz_gfx_blend_presets preset)
{
    D3D12_RENDER_TARGET_BLEND_DESC desc = {0};
    desc.BlendEnable                    = TRUE;
    desc.RenderTargetWriteMask          = D3D12_COLOR_WRITE_ENABLE_ALL;

    switch (preset) {
        case RZ_GFX_BLEND_PRESET_ADDITIVE:
            desc.SrcBlend       = D3D12_BLEND_SRC_ALPHA;
            desc.DestBlend      = D3D12_BLEND_INV_SRC_ALPHA;
            desc.BlendOp        = D3D12_BLEND_OP_ADD;
            desc.SrcBlendAlpha  = D3D12_BLEND_ONE;
            desc.DestBlendAlpha = D3D12_BLEND_ONE;
            desc.BlendOpAlpha   = D3D12_BLEND_OP_ADD;
            break;
        case RZ_GFX_BLEND_PRESET_ALPHA_BLEND:
            desc.SrcBlend       = D3D12_BLEND_SRC_ALPHA;
            desc.DestBlend      = D3D12_BLEND_INV_SRC_ALPHA;
            desc.BlendOp        = D3D12_BLEND_OP_ADD;
            desc.SrcBlendAlpha  = D3D12_BLEND_ONE;
            desc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
            desc.BlendOpAlpha   = D3D12_BLEND_OP_ADD;
            break;
        case RZ_GFX_BLEND_PRESET_SUBTRACTIVE:
            desc.SrcBlend       = D3D12_BLEND_SRC_ALPHA;
            desc.DestBlend      = D3D12_BLEND_ONE;
            desc.BlendOp        = D3D12_BLEND_OP_REV_SUBTRACT;
            desc.SrcBlendAlpha  = D3D12_BLEND_ONE;
            desc.DestBlendAlpha = D3D12_BLEND_ONE;
            desc.BlendOpAlpha   = D3D12_BLEND_OP_REV_SUBTRACT;
            break;
        case RZ_GFX_BLEND_PRESET_MULTIPLY:
            desc.SrcBlend       = D3D12_BLEND_DEST_COLOR;
            desc.DestBlend      = D3D12_BLEND_ZERO;
            desc.BlendOp        = D3D12_BLEND_OP_ADD;
            desc.SrcBlendAlpha  = D3D12_BLEND_DEST_ALPHA;
            desc.DestBlendAlpha = D3D12_BLEND_ZERO;
            desc.BlendOpAlpha   = D3D12_BLEND_OP_ADD;
            break;
        case RZ_GFX_BLEND_PRESET_DARKEN:
            desc.SrcBlend       = D3D12_BLEND_ONE;
            desc.DestBlend      = D3D12_BLEND_ONE;
            desc.BlendOp        = D3D12_BLEND_OP_MIN;
            desc.SrcBlendAlpha  = D3D12_BLEND_ONE;
            desc.DestBlendAlpha = D3D12_BLEND_ONE;
            desc.BlendOpAlpha   = D3D12_BLEND_OP_MIN;
            break;
        default:
            desc.BlendEnable = FALSE;
            break;
    }

    return desc;
}

static D3D12_TEXTURE_ADDRESS_MODE dx12_util_translate_address_mode(rz_gfx_texture_address_mode wrapType)
{
    switch (wrapType) {
        case RZ_GFX_TEXTURE_ADDRESS_MODE_WRAP:
            return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case RZ_GFX_TEXTURE_ADDRESS_MODE_REPEAT:
            return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case RZ_GFX_TEXTURE_ADDRESS_MODE_CLAMP:
            return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        case RZ_GFX_TEXTURE_ADDRESS_MODE_BORDER:
            return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        default:
            RAZIX_RHI_ASSERT(false, "[DX12] Unsupported texture wrap type: %d", wrapType);
            return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    }
}

static D3D12_FILTER dx12_util_translate_filter_type(rz_gfx_texture_filter_type minFilter, rz_gfx_texture_filter_type magFilter, rz_gfx_texture_filter_type mipFilter)
{
    // DirectX 12 uses a combined filter enum that specifies min, mag, and mip filtering in one value
    // We need to determine the appropriate D3D12_FILTER based on the combination of filters

    bool minLinear = (minFilter == RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR ||
                      minFilter == RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_NEAREST ||
                      minFilter == RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_LINEAR);

    bool magLinear = (magFilter == RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR ||
                      magFilter == RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_NEAREST ||
                      magFilter == RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_LINEAR);

    bool mipLinear = (mipFilter == RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_LINEAR ||
                      minFilter == RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_LINEAR ||
                      minFilter == RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST_MIPMAP_LINEAR);

    // Handle cases where mipmap filtering is specified in minFilter
    if (minFilter == RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST_MIPMAP_NEAREST ||
        minFilter == RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_NEAREST) {
        mipLinear = false;
    } else if (minFilter == RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST_MIPMAP_LINEAR ||
               minFilter == RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_LINEAR) {
        mipLinear = true;
    }

    // Combine the filter settings into D3D12_FILTER
    if (!minLinear && !magLinear && !mipLinear) {
        return D3D12_FILTER_MIN_MAG_MIP_POINT;
    } else if (!minLinear && !magLinear && mipLinear) {
        return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    } else if (!minLinear && magLinear && !mipLinear) {
        return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
    } else if (!minLinear && magLinear && mipLinear) {
        return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
    } else if (minLinear && !magLinear && !mipLinear) {
        return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
    } else if (minLinear && !magLinear && mipLinear) {
        return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
    } else if (minLinear && magLinear && !mipLinear) {
        return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    } else if (minLinear && magLinear && mipLinear) {
        return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    }

    // Default fallback
    return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
}

static D3D12_FILTER dx12_util_translate_single_filter_type(rz_gfx_texture_filter_type filterType)
{
    // Simplified version for cases where only one filter type is provided
    switch (filterType) {
        case RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST:
            return D3D12_FILTER_MIN_MAG_MIP_POINT;
        case RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR:
            return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        case RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST_MIPMAP_NEAREST:
            return D3D12_FILTER_MIN_MAG_MIP_POINT;
        case RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_NEAREST:
            return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        case RZ_GFX_TEXTURE_FILTER_TYPE_NEAREST_MIPMAP_LINEAR:
            return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
        case RZ_GFX_TEXTURE_FILTER_TYPE_LINEAR_MIPMAP_LINEAR:
            return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        default:
            RAZIX_RHI_ASSERT(false, "[DX12] Unsupported texture filter type: %d", filterType);
            return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    }
}

static D3D12_DESCRIPTOR_HEAP_TYPE dx12_util_descriptor_heap_type(rz_gfx_descriptor_heap_type type)
{
    switch (type) {
        case RZ_GFX_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        case RZ_GFX_DESCRIPTOR_HEAP_TYPE_SAMPLER: return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        case RZ_GFX_DESCRIPTOR_HEAP_TYPE_RTV: return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        case RZ_GFX_DESCRIPTOR_HEAP_TYPE_DSV: return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        default:
            RAZIX_RHI_ASSERT(false, "[DX12] Unsupported descriptor heap type: %d", type);
            return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    }
}

static bool dx12_util_is_descriptor_type_srv(rz_gfx_descriptor_type type)
{
    switch (type) {
        case RZ_GFX_DESCRIPTOR_TYPE_TEXTURE:
        case RZ_GFX_DESCRIPTOR_TYPE_STRUCTURED:
        case RZ_GFX_DESCRIPTOR_TYPE_BYTE_ADDRESS:
        case RZ_GFX_DESCRIPTOR_TYPE_RT_ACCELERATION_STRUCTURE:
            return true;
        default:
            return false;
    }
}

static bool dx12_util_is_descriptor_type_uav(rz_gfx_descriptor_type type)
{
    switch (type) {
        case RZ_GFX_DESCRIPTOR_TYPE_RW_TEXTURE:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_TYPED:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_BYTE_ADDRESS:
        case RZ_GFX_DESCRIPTOR_TYPE_RW_STRUCTURED_COUNTER:
        case RZ_GFX_DESCRIPTOR_TYPE_APPEND_STRUCTURED:
        case RZ_GFX_DESCRIPTOR_TYPE_CONSUME_STRUCTURED:
            return true;
        default:
            return false;
    }
}

static bool dx12_util_is_descriptor_type_cbv(rz_gfx_descriptor_type type)
{
    return type == RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER;
}

static bool dx12_util_is_descriptor_type_sampler(rz_gfx_descriptor_type type)
{
    return type == RZ_GFX_DESCRIPTOR_TYPE_SAMPLER;
}

static D3D12_RESOURCE_BARRIER dx12_util_transition_resource_state(ID3D12Resource* resource, rz_gfx_resource_state before, rz_gfx_resource_state after)
{
    D3D12_RESOURCE_BARRIER barrier = {
        .Type       = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition = {
            .pResource   = resource,
            .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            .StateBefore = dx12_util_res_state_translate(before),
            .StateAfter  = dx12_util_res_state_translate(after),
        }};
    return barrier;
}

static dx12_cmdbuf dx12_util_begin_singletime_cmdlist(void)
{
    dx12_cmdbuf result = {0};

    CHECK_HR(ID3D12Device_CreateCommandAllocator(
        DX12Device,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        &IID_ID3D12CommandAllocator,
        (void**) &result.cmdAlloc));

    CHECK_HR(ID3D12Device_CreateCommandList(
        DX12Device,
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        result.cmdAlloc,
        NULL,
        &IID_ID3D12GraphicsCommandList,
        (void**) &result.cmdList));

    return result;
}

static void dx12_util_end_singletime_cmdlist(dx12_cmdbuf cmdBuf)
{
    CHECK_HR(ID3D12GraphicsCommandList_Close(cmdBuf.cmdList));

    ID3D12CommandQueue* queue   = DX12Context.directQ;
    ID3D12CommandList*  lists[] = {(ID3D12CommandList*) cmdBuf.cmdList};
    ID3D12CommandQueue_ExecuteCommandLists(queue, 1, lists);

    ID3D12Fence* fence      = NULL;
    UINT64       fenceValue = 1;
    HANDLE       fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    CHECK_HR(ID3D12Device_CreateFence(DX12Device, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, (void**) &fence));

    CHECK_HR(ID3D12CommandQueue_Signal(queue, fence, fenceValue));

    if (ID3D12Fence_GetCompletedValue(fence) < fenceValue) {
        CHECK_HR(ID3D12Fence_SetEventOnCompletion(fence, fenceValue, fenceEvent));
        WaitForSingleObject(fenceEvent, INFINITE);
    }

    CloseHandle(fenceEvent);
    ID3D12Fence_Release(fence);

    ID3D12CommandList_Release(cmdBuf.cmdList);
    ID3D12CommandAllocator_Release(cmdBuf.cmdAlloc);
}

static void dx12_freelist_debug_print(const rz_gfx_descriptor_heap* heap)
{
    RAZIX_RHI_ASSERT(heap != NULL, "Descriptor heap cannot be NULL");
    const rz_gfx_descriptor_freelist_allocator* allocator = heap->freeListAllocator;
    RAZIX_RHI_ASSERT(allocator != NULL, "Free list allocator is not initialized");

    printf("=== DX12 Descriptor Heap Free List ===\n");
    printf("Total Free Ranges: %u\n", allocator->numFreeRanges);

    printf("Fragmentation visualization:\n");
    uint32_t total = heap->resource.desc.descriptorHeapDesc.descriptorCount;
    for (uint32_t i = 0; i < total; ++i) {
        bool isFree = false;
        for (uint32_t r = 0; r < allocator->numFreeRanges; ++r) {
            if (i >= allocator->freeRanges[r].start &&
                i < allocator->freeRanges[r].start + allocator->freeRanges[r].numDescriptors) {
                isFree = true;
                break;
            }
        }
        putchar(isFree ? '.' : '#');    // '.' = free, '#' = allocated
    }
    putchar('\n');

    printf("=====================================\n");
}

static void dx12_create_descriptor_freelist_allocator(rz_gfx_descriptor_heap* heap)
{
    RAZIX_RHI_ASSERT(heap != NULL, "Descriptor heap cannot be NULL");
    rz_gfx_descriptor_heap_desc* desc = &heap->resource.desc.descriptorHeapDesc;
    RAZIX_RHI_ASSERT(desc != NULL, "Descriptor heap descriptor cannot be NULL");
    RAZIX_RHI_ASSERT(heap->resource.desc.descriptorHeapDesc.flags & RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_FREELIST == RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_FREELIST, "Descriptor heap must be of type FREE_LIST");
    // Initialize the free list allocator
    heap->freeListAllocator = malloc(sizeof(rz_gfx_descriptor_freelist_allocator));
    RAZIX_RHI_ASSERT(heap->freeListAllocator != NULL, "Failed to allocate memory for free list allocator");

    heap->freeListAllocator->numFreeRanges = 1;

    heap->freeListAllocator->freeRanges = calloc(RAZIX_INITIAL_DESCRIPTOR_NUM_FREE_RANGES, sizeof(rz_gfx_descriptor_free_range));
    RAZIX_RHI_ASSERT(heap->freeListAllocator->freeRanges != NULL, "Failed to allocate memory for free ranges in free list allocator");

    heap->freeListAllocator->capacity      = RAZIX_INITIAL_DESCRIPTOR_NUM_FREE_RANGES;
    heap->freeListAllocator->freeRanges[0] = (rz_gfx_descriptor_free_range) {
        .start          = 0,
        .numDescriptors = desc->descriptorCount,
    };
}

static void dx12_destroy_descriptor_freelist_allocator(rz_gfx_descriptor_heap* heap)
{
    RAZIX_RHI_ASSERT(heap != NULL, "Descriptor heap cannot be NULL");
    if (heap->freeListAllocator) {
        free(heap->freeListAllocator->freeRanges);
        heap->freeListAllocator->freeRanges    = NULL;
        heap->freeListAllocator->numFreeRanges = 0;

        free(heap->freeListAllocator);
        heap->freeListAllocator = NULL;
    }
}

static dx12_descriptor_handles dx12_descriptor_freelist_allocate(rz_gfx_descriptor_heap* heap, uint32_t numDescriptors)
{
    RAZIX_RHI_ASSERT(heap != NULL, "Descriptor heap cannot be NULL");
    RAZIX_RHI_ASSERT(numDescriptors > 0, "Allocation count must be greater than zero");
    rz_gfx_descriptor_freelist_allocator* allocator = heap->freeListAllocator;
    RAZIX_RHI_ASSERT(allocator != NULL, "Free list allocator is not initialized");
    for (uint32_t i = 0; i < allocator->numFreeRanges; i++) {
        rz_gfx_descriptor_free_range* range = &allocator->freeRanges[i];
        if (range->numDescriptors >= numDescriptors) {
            dx12_descriptor_handles handles = {0};
            handles.cpu.ptr                 = heap->dx12.heapStart.cpu.ptr + ((size_t) range->start * heap->dx12.descriptorSize);
            handles.gpu.ptr                 = heap->dx12.heapStart.gpu.ptr + ((size_t) range->start * heap->dx12.descriptorSize);

            if (range->numDescriptors == numDescriptors) {
                // exact fit: remove by swapping with last
                allocator->freeRanges[i] = allocator->freeRanges[--allocator->numFreeRanges];
            } else {
                // shrink from front
                range->start += numDescriptors;
                range->numDescriptors -= numDescriptors;
            }
            return handles;
        }
    }
    RAZIX_RHI_LOG_ERROR("Failed to allocate %u descriptors from free list", numDescriptors);
    dx12_descriptor_handles invalid = {0};
    return invalid;
}

static void dx12_descriptor_freelist_free(rz_gfx_descriptor_heap* heap, dx12_descriptor_handles handles, uint32_t numDescriptors)
{
    RAZIX_RHI_ASSERT(heap != NULL, "Descriptor heap cannot be NULL");
    RAZIX_RHI_ASSERT(numDescriptors > 0, "Free count must be greater than zero");

    rz_gfx_descriptor_freelist_allocator* allocator = heap->freeListAllocator;
    RAZIX_RHI_ASSERT(allocator != NULL, "Free list allocator is not initialized");

    // Create a new free range
    rz_gfx_descriptor_free_range newRange = {
        .start          = ((uint32_t) handles.cpu.ptr - (uint32_t) heap->dx12.heapStart.cpu.ptr) / heap->dx12.descriptorSize,
        .numDescriptors = numDescriptors,
    };

    if (allocator->numFreeRanges >= allocator->capacity) {
        // Double the capacity of the free ranges array
        uint32_t newCapacity  = allocator->capacity ? allocator->capacity * 2 : RAZIX_INITIAL_DESCRIPTOR_NUM_FREE_RANGES;
        allocator->freeRanges = realloc(allocator->freeRanges, newCapacity * sizeof(rz_gfx_descriptor_free_range));
        RAZIX_RHI_ASSERT(allocator->freeRanges != NULL, "Failed to grow free list array");
        // We are fucked!
        if (!allocator->freeRanges)
            RAZIX_RHI_ABORT();
        allocator->capacity = newCapacity;
    }

    // Insert the new range into the free list at the end of the list
    allocator->freeRanges[allocator->numFreeRanges++] = newRange;

    // Sort ranges by start offset
    for (uint32_t i = 0; i < allocator->numFreeRanges - 1; ++i) {
        for (uint32_t j = i + 1; j < allocator->numFreeRanges; ++j) {
            if (allocator->freeRanges[j].start < allocator->freeRanges[i].start) {
                rz_gfx_descriptor_free_range tmp = allocator->freeRanges[i];
                allocator->freeRanges[i]         = allocator->freeRanges[j];
                allocator->freeRanges[j]         = tmp;
            }
        }
    }

    uint32_t mergedCount = 0;
    for (uint32_t i = 0; i < allocator->numFreeRanges; ++i) {
        if (mergedCount > 0) {
            rz_gfx_descriptor_free_range* prev = &allocator->freeRanges[mergedCount - 1];
            rz_gfx_descriptor_free_range* curr = &allocator->freeRanges[i];

            if (prev->start + prev->numDescriptors >= curr->start) {
                uint32_t endCurr = curr->start + curr->numDescriptors;
                uint32_t endPrev = prev->start + prev->numDescriptors;
                if (endCurr > endPrev)
                    prev->numDescriptors = endCurr - prev->start;
                continue;    // Skip adding curr separately
            }
        }
        allocator->freeRanges[mergedCount++] = allocator->freeRanges[i];
    }
    allocator->numFreeRanges = mergedCount;
}

static dx12_descriptor_handles dx12_descriptor_ringbuffer_allocate(rz_gfx_descriptor_heap* heap, uint32_t numDescriptors)
{
    RAZIX_RHI_ASSERT(heap != NULL, "Descriptor heap cannot be NULL");
    RAZIX_RHI_ASSERT(numDescriptors > 0, "Free count must be greater than zero");
    RAZIX_RHI_ASSERT(heap->resource.desc.descriptorHeapDesc.heapType & RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_FREELIST == RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_FREELIST, "Descriptor heap must be of type FREE_LIST");
    rz_gfx_descriptor_heap_desc* desc = &heap->resource.desc.descriptorHeapDesc;
    RAZIX_RHI_ASSERT(desc != NULL, "Descriptor heap descriptor cannot be NULL");

    uint32_t freeSpace = 0;
    uint32_t capacity  = desc->descriptorCount;

    if (heap->ringBufferHead >= heap->ringBufferTail) {
        freeSpace = capacity - (heap->ringBufferHead - heap->ringBufferTail);
    } else {
        freeSpace = heap->ringBufferTail - heap->ringBufferHead;
    }

    // We are fucked!
    if (heap->isFull || numDescriptors > capacity) {
        RAZIX_RHI_LOG_ERROR("Failed to allocate %u descriptors from ringbuffer, we are full!", numDescriptors);
        RAZIX_RHI_ABORT();
        dx12_descriptor_handles invalid = {0};
        return invalid;
    }

    uint32_t allocatedStart = heap->ringBufferHead;
    heap->ringBufferHead    = (heap->ringBufferHead + numDescriptors) % capacity;

    if (heap->ringBufferHead == heap->ringBufferTail)
        heap->isFull = true;

    dx12_descriptor_handles handles = {0};
    handles.cpu.ptr                 = heap->dx12.heapStart.cpu.ptr + ((size_t) allocatedStart * heap->dx12.descriptorSize);
    handles.gpu.ptr                 = heap->dx12.heapStart.gpu.ptr + ((size_t) allocatedStart * heap->dx12.descriptorSize);
    return handles;
}

static void dx12_descriptor_ringbuffer_free(rz_gfx_descriptor_heap* heap, uint32_t numDescriptors)
{
    RAZIX_RHI_ASSERT(heap != NULL, "Descriptor heap cannot be NULL");
    RAZIX_RHI_ASSERT(numDescriptors > 0, "Free count must be greater than zero");
    // In ring buffer, we don't actually free the descriptors, we just update the tail
    heap->ringBufferTail = (heap->ringBufferTail + numDescriptors) % heap->resource.desc.descriptorHeapDesc.descriptorCount;
    heap->isFull         = false;
}

static dx12_descriptor_handles dx12_descriptor_allocate_handle(rz_gfx_descriptor_heap* heap, uint32_t numDescriptors)
{
    if (heap->resource.desc.descriptorHeapDesc.flags & RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_FREELIST == RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_FREELIST)
        return dx12_descriptor_freelist_allocate(heap, numDescriptors);
    else
        return dx12_descriptor_ringbuffer_allocate(heap, numDescriptors);
}

static void dx12_descriptor_free_handle(rz_gfx_descriptor_heap* heap, dx12_descriptor_handles handle, uint32_t numDescriptors)
{
    if (heap->resource.desc.descriptorHeapDesc.flags & RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_FREELIST == RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_FREELIST)
        dx12_descriptor_freelist_free(heap, handle, numDescriptors);
    else
        dx12_descriptor_ringbuffer_free(heap, numDescriptors);
}

static D3D12_SRV_DIMENSION dx12_util_texture_type_srv_dim(rz_gfx_texture_type type)
{
    switch (type) {
        case RZ_GFX_TEXTURE_TYPE_1D:
            return D3D12_SRV_DIMENSION_TEXTURE1D;
        case RZ_GFX_TEXTURE_TYPE_1D_ARRAY:
            return D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
        case RZ_GFX_TEXTURE_TYPE_2D:
            return D3D12_SRV_DIMENSION_TEXTURE2D;
        case RZ_GFX_TEXTURE_TYPE_2D_ARRAY:
            return D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        case RZ_GFX_TEXTURE_TYPE_3D:
            return D3D12_SRV_DIMENSION_TEXTURE3D;
        case RZ_GFX_TEXTURE_TYPE_CUBE:
            return D3D12_SRV_DIMENSION_TEXTURECUBE;
        case RZ_GFX_TEXTURE_TYPE_CUBE_ARRAY:
            return D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
        default:
            RAZIX_RHI_ABORT();
            return D3D12_SRV_DIMENSION_UNKNOWN;
    }
}

static D3D12_SHADER_RESOURCE_VIEW_DESC dx12_create_texture_srv(const rz_gfx_texture_view_desc* desc, const rz_gfx_texture_desc* textureDesc)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {0};
    srvDesc.Format                          = dx12_util_rz_gfx_format_to_dxgi_format(desc->pTexture->resource.desc.textureDesc.format);
    srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    switch (textureDesc->textureType) {
        case RZ_GFX_TEXTURE_TYPE_1D:
            srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE1D;
            srvDesc.Texture1D.MostDetailedMip     = desc->baseMip;
            srvDesc.Texture1D.MipLevels           = textureDesc->mipLevels;
            srvDesc.Texture1D.ResourceMinLODClamp = 0.0f;
            break;
        case RZ_GFX_TEXTURE_TYPE_2D:
            srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip     = desc->baseMip;
            srvDesc.Texture2D.MipLevels           = textureDesc->mipLevels;
            srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
            break;
        case RZ_GFX_TEXTURE_TYPE_3D:
            srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE3D;
            srvDesc.Texture3D.MostDetailedMip     = desc->baseMip;
            srvDesc.Texture3D.MipLevels           = textureDesc->mipLevels;
            srvDesc.Texture3D.ResourceMinLODClamp = 0.0f;
            break;
        case RZ_GFX_TEXTURE_TYPE_CUBE:
            srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURECUBE;
            srvDesc.TextureCube.MostDetailedMip     = desc->baseMip;
            srvDesc.TextureCube.MipLevels           = textureDesc->mipLevels;
            srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
            break;
        case RZ_GFX_TEXTURE_TYPE_CUBE_ARRAY:
            srvDesc.ViewDimension                        = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
            srvDesc.TextureCubeArray.MostDetailedMip     = desc->baseMip;
            srvDesc.TextureCubeArray.MipLevels           = textureDesc->mipLevels;
            srvDesc.TextureCubeArray.ResourceMinLODClamp = 0.0f;
            srvDesc.TextureCubeArray.First2DArrayFace    = desc->baseArrayLayer;
            srvDesc.TextureCubeArray.NumCubes            = textureDesc->arraySize;
            break;
        case RZ_GFX_TEXTURE_TYPE_1D_ARRAY:
            srvDesc.ViewDimension                      = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
            srvDesc.Texture1DArray.MostDetailedMip     = desc->baseMip;
            srvDesc.Texture1DArray.MipLevels           = textureDesc->mipLevels;
            srvDesc.Texture1DArray.ResourceMinLODClamp = 0.0f;
            srvDesc.Texture1DArray.FirstArraySlice     = desc->baseArrayLayer;
            srvDesc.Texture1DArray.ArraySize           = textureDesc->arraySize;
            break;
        case RZ_GFX_TEXTURE_TYPE_2D_ARRAY:
            srvDesc.ViewDimension                      = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
            srvDesc.Texture2DArray.MostDetailedMip     = desc->baseMip;
            srvDesc.Texture2DArray.MipLevels           = textureDesc->mipLevels;
            srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
            srvDesc.Texture2DArray.FirstArraySlice     = desc->baseArrayLayer;
            srvDesc.Texture2DArray.ArraySize           = textureDesc->arraySize;    // 2D array depth is the array size
            break;

        default:
            break;
    }
    return srvDesc;
}

static D3D12_UNORDERED_ACCESS_VIEW_DESC dx12_create_texture_uav(const rz_gfx_texture_view_desc* desc, const rz_gfx_texture_desc* textureDesc)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {0};
    uavDesc.Format                           = dx12_util_rz_gfx_format_to_dxgi_format(desc->pTexture->resource.desc.textureDesc.format);

    switch (textureDesc->textureType) {
        case RZ_GFX_TEXTURE_TYPE_1D:
            uavDesc.ViewDimension      = D3D12_UAV_DIMENSION_TEXTURE1D;
            uavDesc.Texture1D.MipSlice = desc->baseMip;
            break;
        case RZ_GFX_TEXTURE_TYPE_2D:
            uavDesc.ViewDimension        = D3D12_UAV_DIMENSION_TEXTURE2D;
            uavDesc.Texture2D.MipSlice   = desc->baseMip;
            uavDesc.Texture2D.PlaneSlice = 0;    // Plane slice is only used for planar formats
            break;
        case RZ_GFX_TEXTURE_TYPE_3D:
            uavDesc.ViewDimension         = D3D12_UAV_DIMENSION_TEXTURE3D;
            uavDesc.Texture3D.MipSlice    = desc->baseMip;
            uavDesc.Texture3D.FirstWSlice = desc->baseArrayLayer;
            break;
        case RZ_GFX_TEXTURE_TYPE_1D_ARRAY:
            uavDesc.ViewDimension                  = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
            uavDesc.Texture1DArray.MipSlice        = desc->baseMip;
            uavDesc.Texture1DArray.FirstArraySlice = desc->baseArrayLayer;
            uavDesc.Texture1DArray.ArraySize       = textureDesc->arraySize;
            break;
        case RZ_GFX_TEXTURE_TYPE_2D_ARRAY:
            uavDesc.ViewDimension                  = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
            uavDesc.Texture2DArray.MipSlice        = desc->baseMip;
            uavDesc.Texture2DArray.FirstArraySlice = desc->baseArrayLayer;
            uavDesc.Texture2DArray.ArraySize       = textureDesc->arraySize;
            break;
        case RZ_GFX_TEXTURE_TYPE_CUBE:
        case RZ_GFX_TEXTURE_TYPE_CUBE_ARRAY:
        default:
            RAZIX_RHI_LOG_ERROR("Unsupported texture type for UAV creation: %d", textureDesc->textureType);
            break;
    }
    return uavDesc;
}

static D3D12_RENDER_TARGET_VIEW_DESC dx12_create_texture_rtv(const rz_gfx_texture_view_desc* desc, const rz_gfx_texture_desc* textureDesc)
{
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {0};
    rtvDesc.Format                        = dx12_util_rz_gfx_format_to_dxgi_format(desc->pTexture->resource.desc.textureDesc.format);
    switch (textureDesc->textureType) {
        case RZ_GFX_TEXTURE_TYPE_1D:
            rtvDesc.ViewDimension      = D3D12_RTV_DIMENSION_TEXTURE1D;
            rtvDesc.Texture1D.MipSlice = desc->baseMip;
            break;
        case RZ_GFX_TEXTURE_TYPE_2D:
            rtvDesc.ViewDimension      = D3D12_RTV_DIMENSION_TEXTURE2D;
            rtvDesc.Texture2D.MipSlice = desc->baseMip;
            break;
        case RZ_GFX_TEXTURE_TYPE_3D:
            rtvDesc.ViewDimension         = D3D12_RTV_DIMENSION_TEXTURE3D;
            rtvDesc.Texture3D.MipSlice    = desc->baseMip;
            rtvDesc.Texture3D.FirstWSlice = desc->baseArrayLayer;
            break;
        case RZ_GFX_TEXTURE_TYPE_1D_ARRAY:
            rtvDesc.ViewDimension                  = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
            rtvDesc.Texture1DArray.MipSlice        = desc->baseMip;
            rtvDesc.Texture1DArray.FirstArraySlice = desc->baseArrayLayer;
            rtvDesc.Texture1DArray.ArraySize       = textureDesc->arraySize;
            break;
        case RZ_GFX_TEXTURE_TYPE_2D_ARRAY:
            rtvDesc.ViewDimension                  = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
            rtvDesc.Texture2DArray.MipSlice        = desc->baseMip;
            rtvDesc.Texture2DArray.FirstArraySlice = desc->baseArrayLayer;
            rtvDesc.Texture2DArray.ArraySize       = textureDesc->arraySize;
            break;
        case RZ_GFX_TEXTURE_TYPE_CUBE:
        case RZ_GFX_TEXTURE_TYPE_CUBE_ARRAY:
        default:
            RAZIX_RHI_LOG_ERROR("Unsupported texture type for RTV creation: %d", textureDesc->textureType);
            break;
    }
    return rtvDesc;
}

static D3D12_DEPTH_STENCIL_VIEW_DESC dx12_create_texture_dsv(const rz_gfx_texture_view_desc* desc, const rz_gfx_texture_desc* textureDesc)
{
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {0};
    dsvDesc.Format                        = dx12_util_rz_gfx_format_to_dxgi_format(desc->pTexture->resource.desc.textureDesc.format);
    switch (textureDesc->textureType) {
        case RZ_GFX_TEXTURE_TYPE_1D:
            dsvDesc.ViewDimension      = D3D12_DSV_DIMENSION_TEXTURE1D;
            dsvDesc.Texture1D.MipSlice = desc->baseMip;
            break;
        case RZ_GFX_TEXTURE_TYPE_2D:
            dsvDesc.ViewDimension      = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Texture2D.MipSlice = desc->baseMip;
            break;
        case RZ_GFX_TEXTURE_TYPE_1D_ARRAY:
            dsvDesc.ViewDimension                  = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
            dsvDesc.Texture1DArray.MipSlice        = desc->baseMip;
            dsvDesc.Texture1DArray.FirstArraySlice = desc->baseArrayLayer;
            dsvDesc.Texture1DArray.ArraySize       = textureDesc->arraySize;
            break;
        case RZ_GFX_TEXTURE_TYPE_2D_ARRAY:
            dsvDesc.ViewDimension                  = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
            dsvDesc.Texture2DArray.MipSlice        = desc->baseMip;
            dsvDesc.Texture2DArray.FirstArraySlice = desc->baseArrayLayer;
            dsvDesc.Texture2DArray.ArraySize       = textureDesc->arraySize;
            break;
        case RZ_GFX_TEXTURE_TYPE_3D:
        case RZ_GFX_TEXTURE_TYPE_CUBE:
        case RZ_GFX_TEXTURE_TYPE_CUBE_ARRAY:
        default:
            RAZIX_RHI_LOG_ERROR("Unsupported texture type for DSV creation: %d", textureDesc->textureType);
            break;
    }
    return dsvDesc;
}

static dx12_resview dx12_create_texture_view(const rz_gfx_texture_view_desc* desc, rz_gfx_descriptor_type descriptorType)
{
    dx12_resview dx12_view = {0};

    RAZIX_RHI_ASSERT(desc != NULL, "Texture view desc cannot be NULL");
    RAZIX_RHI_ASSERT(desc->pTexture != NULL, "Texture resource cannot be NULL");
    const rz_gfx_texture* pTexture = desc->pTexture;
    RAZIX_RHI_ASSERT(pTexture != NULL, "Texture resource must be created before creating a texture view");
    const rz_gfx_texture_desc* textureDesc = &pTexture->resource.desc.textureDesc;
    RAZIX_RHI_ASSERT(textureDesc != NULL, "Texture descriptor cannot be NULL");

    bool isTextureRW = rzRHI_IsDescriptorTypeTextureRW(descriptorType);

    if (!isTextureRW && ((pTexture->resource.viewHints & RZ_GFX_RESOURCE_VIEW_FLAG_SRV) == RZ_GFX_RESOURCE_VIEW_FLAG_SRV)) {
        dx12_view.srvDesc = dx12_create_texture_srv(desc, textureDesc);
    } else if (isTextureRW && ((pTexture->resource.viewHints & RZ_GFX_RESOURCE_VIEW_FLAG_UAV) == RZ_GFX_RESOURCE_VIEW_FLAG_UAV)) {
        dx12_view.uavDesc = dx12_create_texture_uav(desc, textureDesc);
    } else if (descriptorType == RZ_GFX_DESCRIPTOR_TYPE_RENDER_TEXTURE && ((pTexture->resource.viewHints & RZ_GFX_RESOURCE_VIEW_FLAG_RTV) == RZ_GFX_RESOURCE_VIEW_FLAG_RTV)) {
        dx12_view.rtvDesc = dx12_create_texture_rtv(desc, textureDesc);
    } else if (descriptorType == RZ_GFX_DESCRIPTOR_TYPE_DEPTH_STENCIL_TEXTURE && ((pTexture->resource.viewHints & RZ_GFX_RESOURCE_VIEW_FLAG_DSV) == RZ_GFX_RESOURCE_VIEW_FLAG_DSV)) {
        dx12_view.dsvDesc = dx12_create_texture_dsv(desc, textureDesc);
    } else {
        RAZIX_RHI_LOG_ERROR("Unsupported texture view descriptor type: %d and view hints: %d", descriptorType, pTexture->resource.viewHints);
        return dx12_view;    // Return empty view
    }
    return dx12_view;    // IDK why MSVC/Clang complained about it even though we have a dangling else
}

static D3D12_CONSTANT_BUFFER_VIEW_DESC dx12_create_buffer_cbv(const rz_gfx_buffer_view_desc* desc, const rz_gfx_buffer_desc* bufferDesc)
{
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {0};
    cbvDesc.BufferLocation                  = ID3D12Resource_GetGPUVirtualAddress(desc->pBuffer->dx12.resource) + desc->offset;
    cbvDesc.SizeInBytes                     = RAZIX_RHI_ALIGN(desc->size, RAZIX_CONSTANT_BUFFER_MIN_ALIGNMENT);
    return cbvDesc;
}

static D3D12_UNORDERED_ACCESS_VIEW_DESC dx12_util_create_buffer_uav(const rz_gfx_buffer_view_desc* desc, const rz_gfx_buffer_desc* bufferDesc)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {0};
    uavDesc.Format                           = dx12_util_rz_gfx_format_to_dxgi_format(desc->format);
    uavDesc.ViewDimension                    = D3D12_UAV_DIMENSION_BUFFER;
    // TODO: Divide them by format size
    uavDesc.Buffer.FirstElement        = desc->offset;
    uavDesc.Buffer.NumElements         = (UINT) desc->size;
    uavDesc.Buffer.StructureByteStride = desc->stride;
    return uavDesc;
}

static dx12_resview dx12_create_buffer_view(const rz_gfx_buffer_view_desc* desc, rz_gfx_descriptor_type descriptorType)
{
    dx12_resview dx12_view = {0};
    RAZIX_RHI_ASSERT(desc != NULL, "Buffer view desc cannot be NULL");
    RAZIX_RHI_ASSERT(desc->pBuffer != NULL, "Buffer resource cannot be NULL");
    const rz_gfx_buffer* pBuffer = desc->pBuffer;
    RAZIX_RHI_ASSERT(pBuffer != NULL, "Buffer resource must be created before creating a buffer view");
    const rz_gfx_buffer_desc* bufferDesc = &pBuffer->resource.desc.bufferDesc;
    RAZIX_RHI_ASSERT(bufferDesc != NULL, "Buffer descriptor cannot be NULL");

    bool isRWBuffer = rzRHI_IsDescriptorTypeBufferRW(descriptorType);

    if (!isRWBuffer && ((pBuffer->resource.viewHints & RZ_GFX_RESOURCE_VIEW_FLAG_CBV) == RZ_GFX_RESOURCE_VIEW_FLAG_CBV)) {
        if (descriptorType == RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER)
            dx12_view.cbvDesc = dx12_create_buffer_cbv(desc, bufferDesc);
    } else if (isRWBuffer && ((pBuffer->resource.viewHints & RZ_GFX_RESOURCE_VIEW_FLAG_UAV) == RZ_GFX_RESOURCE_VIEW_FLAG_UAV)) {
        dx12_view.uavDesc = dx12_util_create_buffer_uav(desc, bufferDesc);
    } else {
        RAZIX_RHI_LOG_ERROR("Unsupported buffer view descriptor type: %d and view hints: %d, Please specify a view flag hint", descriptorType, pBuffer->resource.viewHints);
        return dx12_view;    // Return empty view
    }
    return dx12_view;
}

static dx12_resview dx12_create_sampler_view(const rz_gfx_sampler_view_desc* desc)
{
    dx12_resview dx12_view = {0};
    RAZIX_RHI_ASSERT(desc != NULL, "Sampler view desc cannot be NULL");
    const rz_gfx_sampler* pSampler = desc->pSampler;
    RAZIX_RHI_ASSERT(pSampler != NULL, "Sampler resource must be created before creating a sampler view");
    const rz_gfx_sampler_desc* samplerDesc = &pSampler->resource.desc.samplerDesc;
    RAZIX_RHI_ASSERT(samplerDesc != NULL, "Sampler descriptor cannot be NULL");

    D3D12_SAMPLER_DESC dxsamplerDesc = {0};
    dxsamplerDesc.Filter             = dx12_util_translate_filter_type(samplerDesc->magFilter, samplerDesc->minFilter, samplerDesc->mipFilter);
    dxsamplerDesc.AddressU           = dx12_util_translate_address_mode(samplerDesc->addressModeU);
    dxsamplerDesc.AddressV           = dx12_util_translate_address_mode(samplerDesc->addressModeV);
    dxsamplerDesc.AddressW           = dx12_util_translate_address_mode(samplerDesc->addressModeW);
    dxsamplerDesc.ComparisonFunc     = dx12_util_compare_func(samplerDesc->compareOp);
    dxsamplerDesc.BorderColor[0]     = 1.0f;
    dxsamplerDesc.BorderColor[1]     = 1.0f;
    dxsamplerDesc.BorderColor[2]     = 1.0f;
    dxsamplerDesc.BorderColor[3]     = 1.0f;
    dxsamplerDesc.MaxAnisotropy      = samplerDesc->maxAnisotropy;
    dxsamplerDesc.MipLODBias         = samplerDesc->mipLODBias;
    dxsamplerDesc.MinLOD             = samplerDesc->minLod;
    dxsamplerDesc.MaxLOD             = samplerDesc->maxLod;
    dx12_view.samplerDesc            = dxsamplerDesc;

    return dx12_view;
}

static void dx12_util_upload_pixel_Data(rz_gfx_texture* texture, rz_gfx_texture_desc* desc)
{
    RAZIX_RHI_ASSERT(texture != NULL, "Texture cannot be NULL");
    RAZIX_RHI_ASSERT(desc != NULL, "Texture descriptor cannot be NULL");
    RAZIX_RHI_ASSERT(desc->pPixelData != NULL, "Pixel data cannot be NULL");

    uint32_t bytesPerPixel = rzRHI_GetBytesPerPixel(desc->format);
    uint64_t textureSize   = desc->width * desc->height * desc->depth * bytesPerPixel;

    // Create upload buffer
    D3D12_HEAP_PROPERTIES uploadHeapProps = {0};
    uploadHeapProps.Type                  = D3D12_HEAP_TYPE_UPLOAD;
    uploadHeapProps.CPUPageProperty       = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    uploadHeapProps.MemoryPoolPreference  = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC uploadBufferDesc = {0};
    uploadBufferDesc.Dimension           = D3D12_RESOURCE_DIMENSION_BUFFER;
    uploadBufferDesc.Width               = textureSize;
    uploadBufferDesc.Height              = 1;
    uploadBufferDesc.DepthOrArraySize    = 1;
    uploadBufferDesc.MipLevels           = 1;
    uploadBufferDesc.Format              = DXGI_FORMAT_UNKNOWN;
    uploadBufferDesc.SampleDesc.Count    = 1;
    uploadBufferDesc.Layout              = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    uploadBufferDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;

    ID3D12Resource* uploadBuffer = NULL;
    HRESULT         hr           = ID3D12Device10_CreateCommittedResource(
        DX12Device,
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &uploadBufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        NULL,
        &IID_ID3D12Resource,
        &uploadBuffer);

    if (FAILED(hr)) {
        RAZIX_RHI_LOG_ERROR("Failed to create upload buffer for texture: 0x%08X", hr);
        return;
    }

    void* mappedData = NULL;
    hr               = ID3D12Resource_Map(uploadBuffer, 0, NULL, &mappedData);
    if (FAILED(hr)) {
        RAZIX_RHI_LOG_ERROR("Failed to map upload buffer: 0x%08X", hr);
        ID3D12Resource_Release(uploadBuffer);
        return;
    }

    memcpy(mappedData, desc->pPixelData, textureSize);
    ID3D12Resource_Unmap(uploadBuffer, 0, NULL);

    dx12_cmdbuf cmdBuf = dx12_util_begin_singletime_cmdlist();

    D3D12_TEXTURE_COPY_LOCATION srcLocation        = {0};
    srcLocation.pResource                          = uploadBuffer;
    srcLocation.Type                               = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    srcLocation.PlacedFootprint.Offset             = 0;
    srcLocation.PlacedFootprint.Footprint.Format   = dx12_util_rz_gfx_format_to_dxgi_format(desc->format);
    srcLocation.PlacedFootprint.Footprint.Width    = desc->width;
    srcLocation.PlacedFootprint.Footprint.Height   = desc->height;
    srcLocation.PlacedFootprint.Footprint.Depth    = desc->depth;
    srcLocation.PlacedFootprint.Footprint.RowPitch = desc->width * bytesPerPixel;

    D3D12_TEXTURE_COPY_LOCATION dstLocation = {0};
    dstLocation.pResource                   = texture->dx12.resource;
    dstLocation.Type                        = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLocation.SubresourceIndex            = 0;

    D3D12_RESOURCE_BARRIER barrier = {0};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource   = texture->dx12.resource;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf.cmdList, 1, &barrier);

    ID3D12GraphicsCommandList_CopyTextureRegion(cmdBuf.cmdList, &dstLocation, 0, 0, 0, &srcLocation, NULL);

    // Transition texture back to common state
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_COMMON;
    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf.cmdList, 1, &barrier);

    dx12_util_end_singletime_cmdlist(cmdBuf);

    ID3D12Resource_Release(uploadBuffer);
    RAZIX_RHI_LOG_INFO("Pixel data uploaded successfully");
}

static void dx12_util_upload_buffer_data(rz_gfx_buffer* buffer, rz_gfx_buffer_desc* desc)
{
    RAZIX_RHI_ASSERT(buffer != NULL, "Buffer cannot be NULL");
    RAZIX_RHI_ASSERT(desc != NULL, "Buffer descriptor cannot be NULL");
    RAZIX_RHI_ASSERT(desc->pInitData != NULL, "Initial data cannot be NULL");

    // Create a staging buffer and copy the data to it
    D3D12_HEAP_PROPERTIES uploadHeapProps = {0};
    uploadHeapProps.Type                  = D3D12_HEAP_TYPE_UPLOAD;
    uploadHeapProps.CPUPageProperty       = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    uploadHeapProps.MemoryPoolPreference  = D3D12_MEMORY_POOL_UNKNOWN;
    D3D12_RESOURCE_DESC uploadBufferDesc  = {0};
    uploadBufferDesc.Dimension            = D3D12_RESOURCE_DIMENSION_BUFFER;
    uploadBufferDesc.Width                = desc->sizeInBytes;
    uploadBufferDesc.Height               = 1;
    uploadBufferDesc.DepthOrArraySize     = 1;
    uploadBufferDesc.MipLevels            = 1;
    uploadBufferDesc.Format               = DXGI_FORMAT_UNKNOWN;
    uploadBufferDesc.SampleDesc.Count     = 1;
    uploadBufferDesc.Layout               = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    uploadBufferDesc.Flags                = D3D12_RESOURCE_FLAG_NONE;
    ID3D12Resource* uploadBuffer          = NULL;
    HRESULT         hr                    = ID3D12Device10_CreateCommittedResource(
        DX12Device,
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &uploadBufferDesc,
        D3D12_RESOURCE_STATE_COPY_SOURCE,    // No need to transition upload buffers, already in copy src state
        NULL,
        &IID_ID3D12Resource,
        &uploadBuffer);
    if (FAILED(hr)) {
        RAZIX_RHI_LOG_ERROR("Failed to create upload buffer for buffer: 0x%08X", hr);
        return;
    }
    void* mappedData = NULL;
    hr               = ID3D12Resource_Map(uploadBuffer, 0, NULL, &mappedData);
    if (FAILED(hr)) {
        RAZIX_RHI_LOG_ERROR("Failed to map upload buffer: 0x%08X", hr);
        ID3D12Resource_Release(uploadBuffer);
        return;
    }
    memcpy(mappedData, desc->pInitData, desc->sizeInBytes);
    ID3D12Resource_Unmap(uploadBuffer, 0, NULL);

    dx12_cmdbuf            cmdBuf  = dx12_util_begin_singletime_cmdlist();
    D3D12_RESOURCE_BARRIER barrier = {
        .Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Transition.pResource   = buffer->dx12.resource,
        .Transition.StateBefore = dx12_util_res_state_translate(buffer->resource.currentState),
        .Transition.StateAfter  = D3D12_RESOURCE_STATE_COPY_DEST,
        .Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
    };
    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf.cmdList, 1, &barrier);

    ID3D12GraphicsCommandList_CopyBufferRegion(cmdBuf.cmdList, buffer->dx12.resource, 0, uploadBuffer, 0, desc->sizeInBytes);

    D3D12_RESOURCE_BARRIER restoreBarrier = {
        .Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Transition.pResource   = buffer->dx12.resource,
        .Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST,
        .Transition.StateAfter  = dx12_util_res_state_translate(buffer->resource.currentState),
        .Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
    };
    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf.cmdList, 1, &restoreBarrier);
    dx12_util_end_singletime_cmdlist(cmdBuf);
    ID3D12Resource_Release(uploadBuffer);
    RAZIX_RHI_LOG_INFO("Buffer data uploaded successfully");
}

static void dx12_util_generate_mips(rz_gfx_texture* texture, rz_gfx_texture_desc* desc)
{
    RAZIX_RHI_ASSERT(texture != NULL, "Texture cannot be NULL");
    RAZIX_RHI_ASSERT(desc != NULL, "Texture descriptor cannot be NULL");
    if (desc->mipLevels <= 1) {
        RAZIX_RHI_LOG_INFO("No mipmaps to generate for texture");
        return;
    }

    // Unlike vulkan , D3D12 does not have a built-in mipmap generation function, so we need to use a compute shader
    // We will embed a shader in the engine that generates mipmaps for us, use a String to store the shader code
    // Since RHI is it's own entity, we use a string to store the shader code

    RAZIX_RHI_LOG_ERROR("Mip generation is not implemented for D3D12 yet, using mipmaps will cause a catastrophic failure.");
    RAZIX_RHI_ABORT();
}

static D3D12_RESOURCE_DIMENSION dx12_util_translate_texture_dimension(rz_gfx_texture_type type)
{
    switch (type) {
        case RZ_GFX_TEXTURE_TYPE_1D:
            return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
        case RZ_GFX_TEXTURE_TYPE_2D:
            return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        case RZ_GFX_TEXTURE_TYPE_3D:
            return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        case RZ_GFX_TEXTURE_TYPE_CUBE:
            return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        default:
            RAZIX_RHI_LOG_ERROR("Unsupported texture type for dimension translation: %d", type);
            return D3D12_RESOURCE_DIMENSION_UNKNOWN;
    }
}

//---------------------------------------------------------------------------------------------
// Helper functions

static IDXGIAdapter4* dx12_util_select_best_adapter(IDXGIFactory7* factory, D3D_FEATURE_LEVEL min_feat_level)
{
    IDXGIAdapter4* best_adapter     = NULL;
    size_t         maxDedicatedVRAM = 0;

    for (uint32_t i = 0;; ++i) {
        IDXGIAdapter1* adapter1 = NULL;
        HRESULT        hr       = IDXGIFactory7_EnumAdapters1(factory, i, &adapter1);
        if (hr == DXGI_ERROR_NOT_FOUND)
            break;
        if (FAILED(hr))
            continue;

        IDXGIAdapter4* adapter4 = NULL;
        hr                      = IDXGIAdapter1_QueryInterface(adapter1, &IID_IDXGIAdapter4, &adapter4);
        IDXGIAdapter1_Release(adapter1);

        if (FAILED(hr)) {
            RAZIX_RHI_LOG_ERROR("[D3D12] Failed to query IDXGIAdapter4 (HRESULT = 0x%08X)", (unsigned int) hr);
            continue;
        }

        DXGI_ADAPTER_DESC3 desc = {0};
        hr                      = IDXGIAdapter4_GetDesc3(adapter4, &desc);
        if (FAILED(hr)) {
            RAZIX_RHI_LOG_ERROR("[D3D12] Failed to get adapter description (HRESULT = 0x%08X)", (unsigned int) hr);
            IDXGIAdapter4_Release(adapter4);
            continue;
        }

        if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) {
            IDXGIAdapter4_Release(adapter4);
            continue;    // Skip software adapters (e.g., WARP)
        }

        if (desc.DedicatedVideoMemory > maxDedicatedVRAM) {
            if (best_adapter)
                IDXGIAdapter4_Release(best_adapter);

            best_adapter     = adapter4;
            maxDedicatedVRAM = desc.DedicatedVideoMemory;
        } else {
            IDXGIAdapter4_Release(adapter4);
        }
    }

    if (!best_adapter) {
        RAZIX_RHI_LOG_ERROR("[D3D12] No suitable GPU found for D3D12");
        return NULL;
    }

    DXGI_ADAPTER_DESC3 adapterDesc = {0};
    IDXGIAdapter4_GetDesc3(best_adapter, &adapterDesc);
    RAZIX_RHI_LOG_INFO("Selected Adapter Info:");
    RAZIX_RHI_LOG_INFO("\t -> Name                  : %ls", adapterDesc.Description);
    RAZIX_RHI_LOG_INFO("\t -> VendorID              : %u", adapterDesc.VendorId);
    RAZIX_RHI_LOG_INFO("\t -> DeviceId              : %u", adapterDesc.DeviceId);
    RAZIX_RHI_LOG_INFO("\t -> SubSysId              : %u", adapterDesc.SubSysId);
    RAZIX_RHI_LOG_INFO("\t -> Revision              : %u", adapterDesc.Revision);
    RAZIX_RHI_LOG_INFO("\t -> DedicatedVideoMemory  : %zu", adapterDesc.DedicatedVideoMemory);
    RAZIX_RHI_LOG_INFO("\t -> DedicatedSystemMemory : %zu", adapterDesc.DedicatedSystemMemory);
    RAZIX_RHI_LOG_INFO("\t -> SharedSystemMemory    : %zu", adapterDesc.SharedSystemMemory);

    return best_adapter;
}

static void dx12_util_query_features(dx12_ctx* ctx)
{
    ID3D12Device10*    device = ctx->device10;
    D3D12FeatureCache* f      = &ctx->features;

    ID3D12Device10_CheckFeatureSupport(device, D3D12_FEATURE_D3D12_OPTIONS, &f->options, sizeof(f->options));
    ID3D12Device10_CheckFeatureSupport(device, D3D12_FEATURE_D3D12_OPTIONS1, &f->options1, sizeof(f->options1));
    ID3D12Device10_CheckFeatureSupport(device, D3D12_FEATURE_D3D12_OPTIONS5, &f->options5, sizeof(f->options5));

    f->architecture.NodeIndex = 0;
    ID3D12Device10_CheckFeatureSupport(device, D3D12_FEATURE_ARCHITECTURE1, &f->architecture, sizeof(f->architecture));
    f->isUMA              = f->architecture.UMA;
    f->isCacheCoherentUMA = f->architecture.CacheCoherentUMA;

    f->shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_7;
    if (FAILED(ID3D12Device10_CheckFeatureSupport(device, D3D12_FEATURE_SHADER_MODEL, &f->shaderModel, sizeof(f->shaderModel)))) {
        f->shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_0;
    }

    f->rootSig.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(ID3D12Device10_CheckFeatureSupport(device, D3D12_FEATURE_ROOT_SIGNATURE, &f->rootSig, sizeof(f->rootSig)))) {
        f->rootSig.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    ID3D12Device10_CheckFeatureSupport(device, D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, &f->vaSupport, sizeof(f->vaSupport));

    f->nodeCount = ID3D12Device10_GetNodeCount(device);
}

static void dx12_util_print_device_info(IDXGIAdapter4* adapter)
{
    if (!adapter) {
        RAZIX_RHI_LOG_INFO("[DX12] Invalid IDXGIAdapter4*");
        return;
    }

    DXGI_ADAPTER_DESC3 desc;
    memset(&desc, 0, sizeof(desc));
    IDXGIAdapter4_GetDesc3(adapter, &desc);

    /* Convert wide name to basic ASCII */
    char nameBuf[256];
    {
        size_t i = 0;
        while (desc.Description[i] && i < (sizeof(nameBuf) - 1)) {
            wchar_t wc = desc.Description[i];
            if (wc < 0x80) nameBuf[i] = (char) wc;
            else
                nameBuf[i] = '?';
            ++i;
        }
        nameBuf[i] = '\0';
    }

    /* LUID string */
    char luidStr[32];
    (void) snprintf(luidStr, sizeof(luidStr), "%08X%08X", (uint32_t) desc.AdapterLuid.HighPart, (uint32_t) desc.AdapterLuid.LowPart);

    /* Device type heuristic */
    const char* deviceType = "Discrete GPU";
    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        deviceType = "CPU (Software)";
    else if (desc.DedicatedVideoMemory < (256u << 20) ||
             desc.SharedSystemMemory > desc.DedicatedVideoMemory)
        deviceType = "Integrated GPU";

    /* ------------------------------------------------------------------ */
    /* Driver Version via CheckInterfaceSupport                           */
    /* ------------------------------------------------------------------ */
    LARGE_INTEGER drvVer;
    drvVer.QuadPart = 0;
    {
        IDXGIAdapter* baseAdapter = (IDXGIAdapter*) adapter;
        if (baseAdapter &&
            SUCCEEDED(IDXGIAdapter4_CheckInterfaceSupport(baseAdapter, &IID_IDXGIDevice, &drvVer))) {
            /* Format (per MSDN):
               HighPart: (Major << 16) | Minor
               LowPart : (Build << 16) | Revision
            */
        }
    }
    /* Extract parts */
    unsigned drvMajor    = (unsigned) ((drvVer.HighPart >> 16) & 0xFFFF);
    unsigned drvMinor    = (unsigned) (drvVer.HighPart & 0xFFFF);
    unsigned drvBuild    = (unsigned) ((drvVer.LowPart >> 16) & 0xFFFF);
    unsigned drvRevision = (unsigned) (drvVer.LowPart & 0xFFFF);

    /* ------------------------------------------------------------------ */
    /* Highest Supported D3D Feature Level                                */
    /* ------------------------------------------------------------------ */
    const D3D_FEATURE_LEVEL candidates[] = {
#ifdef D3D_FEATURE_LEVEL_12_2
        D3D_FEATURE_LEVEL_12_2,
#endif
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0};
    const char*       featureLevelStr = "Unknown";
    D3D_FEATURE_LEVEL chosen          = 0;
    {
        size_t i;
        for (i = 0; i < (sizeof(candidates) / sizeof(candidates[0])); ++i) {
            ID3D12Device* tmpDev = 0;
            if (SUCCEEDED(D3D12CreateDevice((IUnknown*) adapter,
                    candidates[i],
                    &IID_ID3D12Device,
                    (void**) &tmpDev))) {
                chosen = candidates[i];
                if (tmpDev) tmpDev->lpVtbl->Release(tmpDev);
                break;
            }
        }
        switch (chosen) {
#ifdef D3D_FEATURE_LEVEL_12_2
            case D3D_FEATURE_LEVEL_12_2: featureLevelStr = "12_2"; break;
#endif
            case D3D_FEATURE_LEVEL_12_1: featureLevelStr = "12_1"; break;
            case D3D_FEATURE_LEVEL_12_0: featureLevelStr = "12_0"; break;
            case D3D_FEATURE_LEVEL_11_1: featureLevelStr = "11_1"; break;
            case D3D_FEATURE_LEVEL_11_0: featureLevelStr = "11_0"; break;
            default: break;
        }
    }

    RAZIX_RHI_LOG_INFO("+======================================================================+");
    RAZIX_RHI_LOG_INFO("|                            DX12 GPU INFO                             |");
    RAZIX_RHI_LOG_INFO("+======================================================================+");
    RAZIX_RHI_LOG_INFO("| API                   : DirectX 12");
    RAZIX_RHI_LOG_INFO("| Device Name           : %s", nameBuf);
    RAZIX_RHI_LOG_INFO("| Vendor                : %s (0x%04X)", rzRHI_GetGPUVendorName(desc.VendorId), desc.VendorId);
    RAZIX_RHI_LOG_INFO("| Device ID             : 0x%04X", desc.DeviceId);
    RAZIX_RHI_LOG_INFO("| SubSys ID             : 0x%04X", desc.SubSysId);
    RAZIX_RHI_LOG_INFO("| Revision              : %u", desc.Revision);
    RAZIX_RHI_LOG_INFO("| Device Type           : %s", deviceType);
    RAZIX_RHI_LOG_INFO("| VendorID              : %u", desc.VendorId);
    RAZIX_RHI_LOG_INFO("| LUID                  : %s", luidStr);
    RAZIX_RHI_LOG_INFO("| API Version           : 12.x (runtime-specific)");
    RAZIX_RHI_LOG_INFO("| Driver Version        : N/A");
    RAZIX_RHI_LOG_INFO("| API                   : Direct3D 12 (Feature Level %s)", featureLevelStr);
    if (drvVer.QuadPart != 0ULL)
        RAZIX_RHI_LOG_INFO("| Driver Version        : %u.%u.%u.%u", drvMajor, drvMinor, drvBuild, drvRevision);
    else
        RAZIX_RHI_LOG_INFO("| Driver Version        : (unavailable)");
    RAZIX_RHI_LOG_INFO("+---------------------------------------------------------------------+");
    RAZIX_RHI_LOG_INFO("| Memory (raw bytes)                                                  |");
    RAZIX_RHI_LOG_INFO("+---------------------------------------------------------------------+");
    RAZIX_RHI_LOG_INFO("| DedicatedVideoMemory  : %llu", (unsigned long long) desc.DedicatedVideoMemory);
    RAZIX_RHI_LOG_INFO("| DedicatedSystemMemory : %llu", (unsigned long long) desc.DedicatedSystemMemory);
    RAZIX_RHI_LOG_INFO("| SharedSystemMemory    : %llu", (unsigned long long) desc.SharedSystemMemory);
    RAZIX_RHI_LOG_INFO("| (Heaps synthesized: DEVICE_LOCAL + SHARED_SYSTEM)                   |");
    RAZIX_RHI_LOG_INFO("+---------------------------------------------------------------------+");
    RAZIX_RHI_LOG_INFO("| Limits / Features (Not Queried - Adapter Only Context)              |");
    RAZIX_RHI_LOG_INFO("+---------------------------------------------------------------------+");
    RAZIX_RHI_LOG_INFO("| maxImageDimension2D    : N/A");
    RAZIX_RHI_LOG_INFO("| maxImageDimension3D    : N/A");
    RAZIX_RHI_LOG_INFO("| maxUniformBufferRange  : N/A");
    RAZIX_RHI_LOG_INFO("| maxStorageBufferRange  : N/A");
    RAZIX_RHI_LOG_INFO("| maxPushConstantsSize   : N/A");
    RAZIX_RHI_LOG_INFO("| maxBoundDescriptorSets : N/A");
    RAZIX_RHI_LOG_INFO("| maxViewports           : N/A");
    RAZIX_RHI_LOG_INFO("| maxComputeWorkGroupSize: N/A");
    RAZIX_RHI_LOG_INFO("| geometryShader         : (assumed 1)");
    RAZIX_RHI_LOG_INFO("| tessellationShader     : (assumed 1)");
    RAZIX_RHI_LOG_INFO("| multiViewport          : (assumed 1)");
    RAZIX_RHI_LOG_INFO("| samplerAnisotropy      : (assumed 1)");
    RAZIX_RHI_LOG_INFO("| shaderFloat64          : (varies, assume 1)");
    RAZIX_RHI_LOG_INFO("| shaderInt64            : (varies, assume 1)");
    RAZIX_RHI_LOG_INFO("| robustBufferAccess     : N/A");
    RAZIX_RHI_LOG_INFO("+======================================================================+");
}

#ifdef RAZIX_DEBUG
// Before Device
static void dx12_util_register_debug_interface(dx12_ctx* ctx)
{
    if (SUCCEEDED(D3D12GetDebugInterface(&IID_ID3D12Debug3, (void**) &ctx->d3dDebug3))) {
        ID3D12Debug3_EnableDebugLayer(ctx->d3dDebug3);
        // THIS IS SLOW AF! DISABLE IT ON NVIDIA GPUs
        //ID3D12Debug3_SetEnableGPUBasedValidation(backend->d3d12_debug, TRUE);
        RAZIX_RHI_LOG_INFO("D3D12 debug layer and GPU-based validation enabled");
    } else {
        RAZIX_RHI_LOG_WARN("D3D12 debug interface not available. Debug layer not enabled.");
    }
}

// After Device
static void dx12_util_d3d12_register_info_queue(dx12_ctx* ctx)
{
    if (!ctx->device10) {
        RAZIX_RHI_LOG_ERROR("[D3D12] D3D12 device is NULL; can't register info queue.");
        return;
    }

    // This increases the device refcount.
    if (CHECK_HR(ID3D12InfoQueue_QueryInterface(ctx->device10, &IID_ID3D12InfoQueue, (void**) &ctx->d3dInfoQ))) {
        ID3D12InfoQueue* q = ctx->d3dInfoQ;

        ID3D12InfoQueue_SetBreakOnSeverity(q, D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        ID3D12InfoQueue_SetBreakOnSeverity(q, D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);

        RAZIX_RHI_LOG_INFO("D3D12 info queue registered and debug message filters installed");
    } else {
        RAZIX_RHI_LOG_WARN("Failed to query ID3D12InfoQueue interface. Validation messages will not be captured.");
    }
}

static void dx12_util_dxgi_register_info_queue(dx12_ctx* ctx)
{
    if (SUCCEEDED(DXGIGetDebugInterface1(0, &IID_IDXGIInfoQueue, (void**) &ctx->dxgiInfoQ))) {
        IDXGIInfoQueue* q = ctx->dxgiInfoQ;

        IDXGIInfoQueue_SetBreakOnSeverity(q, DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
        IDXGIInfoQueue_SetBreakOnSeverity(q, DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, TRUE);

        RAZIX_RHI_LOG_INFO("DXGI debug info queue registered");
    } else {
        RAZIX_RHI_LOG_WARN("DXGI info queue not available. No message filtering.");
    }
}

static void dx12_util_track_dxgi_liveobjects(dx12_ctx* ctx)
{
    RAZIX_RHI_LOG_WARN("Tracking live DXGI objects. This will report all live objects at the end of the program.");
    if (SUCCEEDED(DXGIGetDebugInterface1(0, &IID_IDXGIDebug, (void**) &ctx->dxgiDebug))) {
        IDXGIDebug_ReportLiveObjects(
            ctx->dxgiDebug,
            DXGI_DEBUG_ALL,
            (DXGI_DEBUG_RLO_FLAGS) (DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
        RAZIX_RHI_LOG_INFO("DXGI live object report completed");
        IDXGIDebug_Release(ctx->dxgiDebug);
    }
}

static void dx12_util_destroy_debug_handles(dx12_ctx* ctx)
{
    if (ctx->d3dDebug3) {
        ID3D12Debug3_Release(ctx->d3dDebug3);
        ctx->d3dDebug3 = NULL;
    }
    if (ctx->d3dInfoQ) {
        ID3D12InfoQueue_Release(ctx->d3dInfoQ);
        ctx->d3dInfoQ = NULL;
    }
    if (ctx->dxgiInfoQ) {
        IDXGIInfoQueue_Release(ctx->dxgiInfoQ);
        ctx->dxgiInfoQ = NULL;
    }
    // TODO: Done after device destroy
    if (ctx->dxgiDebug) {
        IDXGIDebug_Release(ctx->dxgiDebug);
        ctx->dxgiDebug = NULL;
    }
}
#endif

static void dx12_util_update_swapchain_rtvs(rz_gfx_swapchain* sc)
{
    sc->imageCount = RAZIX_MIN_SWAP_IMAGES_COUNT;
    ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(sc->dx12.rtvHeap, &sc->dx12.rtvHeapStart.cpu);
    RAZIX_RHI_ASSERT(sc->dx12.rtvHeapStart.cpu.ptr != 0, "Swapchain RTV heap start CPU handle is null");

    for (uint32_t i = 0; i < sc->imageCount; i++) {
        ID3D12Resource* d3dresource = NULL;
        HRESULT         hr          = IDXGISwapChain4_GetBuffer(sc->dx12.swapchain4, i, &IID_ID3D12Resource, (void**) &d3dresource);
        if (FAILED(hr)) {
            RAZIX_RHI_LOG_ERROR("[D3D12] Failed to get backbuffer %u from swapchain (HRESULT = 0x%08X)", i, (unsigned int) hr);

            for (uint32_t j = 0; j < i; ++j) {
                if (sc->backbuffers[j].dx12.resource) {
                    ID3D12Resource_Release(sc->backbuffers[j].dx12.resource);
                    sc->backbuffers[j].dx12.resource = NULL;
                }
            }

            ID3D12DescriptorHeap_Release(sc->dx12.rtvHeap);
            IDXGISwapChain4_Release(sc->dx12.swapchain4);
            memset(sc, 0, sizeof(dx12_swapchain));
            return;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = sc->dx12.rtvHeapStart.cpu;
        rtvHandle.ptr += (size_t) i * ID3D12Device10_GetDescriptorHandleIncrementSize(DX12Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        ID3D12Device10_CreateRenderTargetView(DX12Device, d3dresource, NULL, rtvHandle);
#ifdef RAZIX_DEBUG
        ID3D12Resource_SetName(d3dresource, dx12_util_string_to_lpcwstr("Swapchain Image"));
#endif

        // This is the only place where a RZ_RESOURCE is manually created, instead of using the RZResourceManager
        rz_gfx_texture texture   = {0};
        dx12_texture   dxtexture = {0};
        snprintf(texture.resource.pName, RAZIX_MAX_RESOURCE_NAME_CHAR, "$SWAPCHAIN_IMAGE$_%u", i);
        texture.resource.handle                       = (rz_handle) {i, i};
        texture.resource.viewHints                    = RZ_GFX_RESOURCE_VIEW_FLAG_RTV;
        texture.resource.type                         = RZ_GFX_RESOURCE_TYPE_TEXTURE;
        dxtexture.resource                            = d3dresource;
        texture.dx12                                  = dxtexture;
        texture.resource.desc.textureDesc.height      = sc->height;
        texture.resource.desc.textureDesc.width       = sc->width;
        texture.resource.desc.textureDesc.depth       = 1;
        texture.resource.desc.textureDesc.arraySize   = 1;
        texture.resource.desc.textureDesc.mipLevels   = 1;
        texture.resource.desc.textureDesc.format      = RAZIX_SWAPCHAIN_FORMAT;
        texture.resource.desc.textureDesc.textureType = RZ_GFX_TEXTURE_TYPE_2D;
        sc->backbuffers[i]                            = texture;

        rz_gfx_resource_view view = {0};
        snprintf(view.resource.pName, RAZIX_MAX_RESOURCE_NAME_CHAR, "$SWAPCHAIN_RES_VIEW$_%u", i);
        view.resource.handle       = (rz_handle) {i, i};
        view.resource.type         = RZ_GFX_RESOURCE_TYPE_RESOURCE_VIEW;
        view.dx12.rtv.cpu          = rtvHandle;
        sc->backbuffersResViews[i] = view;

        TAG_OBJECT(d3dresource, "Swapchain Backbuffer Resource");
    }
}

static void dx12_util_create_backbuffers(rz_gfx_swapchain* sc)
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {0};
    rtvHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.NumDescriptors             = RAZIX_MIN_SWAP_IMAGES_COUNT;
    rtvHeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    HRESULT hr = ID3D12Device10_CreateDescriptorHeap(DX12Device, &rtvHeapDesc, &IID_ID3D12DescriptorHeap, (void**) &sc->dx12.rtvHeap);
    if (FAILED(hr) || sc->dx12.rtvHeap == NULL) {
        RAZIX_RHI_LOG_ERROR("[D3D12] Failed to create RTV descriptor heap (HRESULT = 0x%08X)", (unsigned int) hr);
        if (sc->dx12.swapchain4)
            IDXGISwapChain4_Release(sc->dx12.swapchain4);
        memset(sc, 0, sizeof(dx12_swapchain));
        return;
    }
    TAG_OBJECT(sc->dx12.rtvHeap, "Swapchain Heap");

    dx12_util_update_swapchain_rtvs(sc);
}

static void dx12_util_destroy_backbuffers(rz_gfx_swapchain* sc)
{
    for (uint32_t i = 0; i < sc->imageCount; ++i) {
        if (sc->backbuffers[i].dx12.resource) {
            ID3D12Resource_Release(sc->backbuffers[i].dx12.resource);
            sc->backbuffers[i].dx12.resource = NULL;
        }
    }

    if (sc->dx12.rtvHeap) {
        ID3D12DescriptorHeap_Release(sc->dx12.rtvHeap);
        sc->dx12.rtvHeap = NULL;
    }
}

static void dx12_util_create_command_signatures(dx12_ctx* ctx)
{
    // Indirect Draw
    D3D12_INDIRECT_ARGUMENT_DESC drawArgs    = {0};
    drawArgs.Type                            = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;
    D3D12_COMMAND_SIGNATURE_DESC drawSigDesc = {0};
    drawSigDesc.pArgumentDescs               = &drawArgs;
    drawSigDesc.NumArgumentDescs             = 1;
    drawSigDesc.ByteStride                   = sizeof(D3D12_DRAW_INDEXED_ARGUMENTS);
    CHECK_HR(ID3D12Device10_CreateCommandSignature(DX12Device, &drawSigDesc, NULL, &IID_ID3D12CommandSignature, (void**) &ctx->drawIndirectSignature));
    if (ctx->drawIndirectSignature == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create Draw Indirect Command Signature");
        return;
    }
    TAG_OBJECT(ctx->drawIndirectSignature, "Draw Indirect Command Signature");

    D3D12_INDIRECT_ARGUMENT_DESC drawIndexedArgs       = {0};
    drawIndexedArgs.Type                               = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
    D3D12_COMMAND_SIGNATURE_DESC drawIndexedCmdSigDesc = {0};
    drawIndexedCmdSigDesc.pArgumentDescs               = &drawIndexedArgs;
    drawIndexedCmdSigDesc.NumArgumentDescs             = 1;
    drawIndexedCmdSigDesc.ByteStride                   = sizeof(D3D12_DRAW_INDEXED_ARGUMENTS);
    drawIndexedCmdSigDesc.NodeMask                     = 0;

    CHECK_HR(ID3D12Device_CreateCommandSignature(
        DX12Device,
        &drawIndexedCmdSigDesc,
        NULL,
        &IID_ID3D12CommandSignature,
        (void**) &ctx->drawIndexedIndirectSignature));
    if (ctx->drawIndexedIndirectSignature == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create Draw Indexed Indirect Command Signature");
        return;
    }
    TAG_OBJECT(ctx->drawIndexedIndirectSignature, "Draw Indexed Indirect Command Signature");

    // Indirect Dispatch
    D3D12_INDIRECT_ARGUMENT_DESC dispatchArgs    = {0};
    dispatchArgs.Type                            = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;
    D3D12_COMMAND_SIGNATURE_DESC dispatchSigDesc = {0};
    dispatchSigDesc.pArgumentDescs               = &dispatchArgs;
    dispatchSigDesc.NumArgumentDescs             = 1;
    dispatchSigDesc.ByteStride                   = sizeof(D3D12_DISPATCH_ARGUMENTS);
    CHECK_HR(ID3D12Device10_CreateCommandSignature(DX12Device, &dispatchSigDesc, NULL, &IID_ID3D12CommandSignature, (void**) &ctx->dispatchIndirectSignature));
    if (ctx->dispatchIndirectSignature == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create Dispatch Indirect Command Signature");
        return;
    }
    TAG_OBJECT(ctx->dispatchIndirectSignature, "Dispatch Indirect Command Signature");
}

static void dx12_util_destroy_command_signatures(dx12_ctx* ctx)
{
    if (ctx->drawIndirectSignature) {
        ID3D12CommandSignature_Release(ctx->drawIndirectSignature);
        ctx->drawIndirectSignature = NULL;
    }
    if (ctx->drawIndexedIndirectSignature) {
        ID3D12CommandSignature_Release(ctx->drawIndexedIndirectSignature);
        ctx->drawIndexedIndirectSignature = NULL;
    }
    if (ctx->dispatchIndirectSignature) {
        ID3D12CommandSignature_Release(ctx->dispatchIndirectSignature);
        ctx->dispatchIndirectSignature = NULL;
    }
}

//---------------------------------------------------------------------------------------------
// Public API functions

static void dx12_GlobalCtxInit(rz_gfx_context_desc init)
{
    RAZIX_RHI_LOG_INFO("Creating DXGI factory");

    g_GfxCtx.ctxDesc = init;

    UINT createFactoryFlags = 0;
#if defined(RAZIX_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    CHECK_HR(CreateDXGIFactory2(createFactoryFlags, &IID_IDXGIFactory7, (void**) &DX12Context.factory7));
    if (DX12Context.factory7 == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create DXGI Factory7");
        return;
    }

    // Set the minimum requested feature level
    DX12Context.featureLevel = D3D_FEATURE_LEVEL_12_0;

    DX12Context.adapter4 = dx12_util_select_best_adapter(DX12Context.factory7, DX12Context.featureLevel);
    if (DX12Context.adapter4 == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to select a suitable D3D12 adapter");
        return;
    }

#ifdef RAZIX_DEBUG
    // We register D3D12Debug interface before device create
    if (init.opts.enableValidation)
        dx12_util_register_debug_interface(&DX12Context);
#endif

    // Create the device
    RAZIX_RHI_LOG_INFO("Creating D3D12 Device");
    CHECK_HR(D3D12CreateDevice((IUnknown*) DX12Context.adapter4, DX12Context.featureLevel, &IID_ID3D12Device10, (void**) &DX12Context.device10));
    if (DX12Context.device10 == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Device");
        return;
    }

#ifdef RAZIX_DEBUG
    if (init.opts.enableValidation) {
        // Register the D3D12 info queue after device creation
        dx12_util_d3d12_register_info_queue(&DX12Context);
        // Register the DXGI info queue
        dx12_util_dxgi_register_info_queue(&DX12Context);
    }
    // Print the D3D12 features
    dx12_util_query_features(&DX12Context);
    dx12_util_print_device_info(DX12Context.adapter4);
    RAZIX_RHI_LOG_INFO("D3D12 Device created successfully");
#else
    RAZIX_RHI_LOG_INFO("D3D12 Device created successfully without debug features");
#endif

    // Create global command queue
    D3D12_COMMAND_QUEUE_DESC desc = {0};
    desc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority                 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask                 = 0;
    CHECK_HR(ID3D12Device10_CreateCommandQueue(DX12Device, &desc, &IID_ID3D12CommandQueue, &DX12Context.directQ));
    if (DX12Context.directQ == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Command Queue");
        return;
    }
    RAZIX_RHI_LOG_INFO("Created Global Direct Command Q");
    TAG_OBJECT(DX12Context.directQ, "Direct Command Q");

    g_GraphicsFeatures.support.TesselateTerrain     = false;
    g_GraphicsFeatures.support.BindlessRendering    = DX12Context.features.options.ResourceBindingTier >= D3D12_RESOURCE_BINDING_TIER_3;
    g_GraphicsFeatures.support.WaveIntrinsics       = true;
    g_GraphicsFeatures.support.ShaderModel6         = DX12Context.features.shaderModel.HighestShaderModel >= D3D_SHADER_MODEL_6_0;
    g_GraphicsFeatures.support.NullIndexDescriptors = DX12Context.features.options5.SRVOnlyTiledResourceTier3;
    g_GraphicsFeatures.support.TimelineSemaphores   = true;
    g_GraphicsFeatures.MaxBindlessTextures          = 4096;    // limit can be increased if needed
    g_GraphicsFeatures.MinLaneWidth                 = DX12Context.features.options1.WaveLaneCountMin;
    g_GraphicsFeatures.MaxLaneWidth                 = DX12Context.features.options1.WaveLaneCountMax;

    dx12_util_create_command_signatures(&DX12Context);

    RAZIX_RHI_LOG_INFO("D3D12 Global context initialized successfully");
}

static void dx12_GlobalCtxDestroy(void)
{
    dx12_util_destroy_command_signatures(&DX12Context);

    if (DX12Context.directQ) {
        ID3D12CommandQueue_Release(DX12Context.directQ);
        DX12Context.directQ = NULL;
    }

    if (DX12Context.device10) {
        ID3D12Device10_Release(DX12Context.device10);
        DX12Context.device10 = NULL;
    }

    if (DX12Context.adapter4) {
        IDXGIAdapter4_Release(DX12Context.adapter4);
        DX12Context.adapter4 = NULL;
    }

    if (DX12Context.factory7) {
        IDXGIFactory7_Release(DX12Context.factory7);
        DX12Context.factory7 = NULL;
    }

#ifdef RAZIX_DEBUG
    if (g_GfxCtx.ctxDesc.opts.enableValidation) {
        dx12_util_destroy_debug_handles(&DX12Context);
        dx12_util_track_dxgi_liveobjects(&DX12Context);
    }
#endif

    RAZIX_RHI_LOG_INFO("DX12 RHI backend destroyed");
}

static void dx12_CreateSyncobjFn(void* where, rz_gfx_syncobj_type type)
{
    rz_gfx_syncobj* syncobj = (rz_gfx_syncobj*) where;
    syncobj->type           = type;

    // Create a fence for synchronization
    CHECK_HR(ID3D12Device10_CreateFence(DX12Device, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, (void**) &syncobj->dx12.fence));
    if (syncobj->dx12.fence == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Fence");
        return;
    }

    if (type == RZ_GFX_SYNCOBJ_TYPE_CPU || type == RZ_GFX_SYNCOBJ_TYPE_TIMELINE) {
        // Create an event handle for the fence
        syncobj->dx12.eventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (syncobj->dx12.eventHandle == NULL) {
            RAZIX_RHI_LOG_ERROR("Failed to create event handle for D3D12 Fence");
            ID3D12Fence_Release(syncobj->dx12.fence);
            syncobj->dx12.fence = NULL;
        }
    }

    TAG_OBJECT(DX12Context.directQ, "Syncobj");
}

static void dx12_DestroySyncobjFn(rz_gfx_syncobj* syncobj)
{
    if (syncobj->dx12.fence) {
        ID3D12Fence_Release(syncobj->dx12.fence);
        syncobj->dx12.fence = NULL;
    }

    if (syncobj->dx12.eventHandle) {
        CloseHandle(syncobj->dx12.eventHandle);
        syncobj->dx12.eventHandle = NULL;
    }
}

static void dx12_CreateSwapchain(void* where, void* nativeWindowHandle, uint32_t width, uint32_t height)
{
    // we typically get a GLFWwinsow*
    if (nativeWindowHandle == NULL) {
        RAZIX_RHI_LOG_ERROR("Native window handle is NULL, cannot create swapchain");
        return;
    }
    if (width == 0 || height == 0) {
        RAZIX_RHI_LOG_ERROR("Invalid swapchain dimensions: %ux%u", width, height);
        return;
    }

    rz_gfx_swapchain* swapchain = (rz_gfx_swapchain*) where;
    memset(swapchain, 0, sizeof(rz_gfx_swapchain));
    swapchain->width       = width;
    swapchain->height      = height;
    swapchain->dx12.window = *(HWND*) nativeWindowHandle;
    swapchain->imageCount  = RAZIX_MIN_SWAP_IMAGES_COUNT;

    DXGI_SWAP_CHAIN_DESC1 desc = {0};
    desc.Width                 = width;
    desc.Height                = height;
    desc.Format                = dx12_util_rz_gfx_format_to_dxgi_format(RAZIX_SWAPCHAIN_FORMAT);
    desc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount           = RAZIX_MIN_SWAP_IMAGES_COUNT;
    desc.Scaling               = DXGI_SCALING_STRETCH;
    desc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.SampleDesc.Count      = 1;    // No MSAA
    desc.SampleDesc.Quality    = 0;
    desc.Flags                 = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    IDXGISwapChain1* swapchain1 = NULL;
    CHECK_HR(IDXGIFactory7_CreateSwapChainForHwnd(DX12Context.factory7, (IUnknown*) DX12Context.directQ, (HWND) swapchain->dx12.window, &desc, NULL, NULL, &swapchain1));
    if (swapchain1 == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Swapchain");
        return;
    }

    CHECK_HR(IDXGISwapChain1_QueryInterface(swapchain1, &IID_IDXGISwapChain4, (void**) &swapchain->dx12.swapchain4));
    IDXGISwapChain1_Release(swapchain1);

    dx12_util_create_backbuffers(swapchain);
}

static void dx12_DestroySwapchain(rz_gfx_swapchain* sc)
{
    dx12_util_destroy_backbuffers(sc);

    if (sc->dx12.swapchain4) {
        IDXGISwapChain4_Release(sc->dx12.swapchain4);
        sc->dx12.swapchain4 = NULL;
    }

    sc->dx12.imageCount   = 0;
    sc->currBackBufferIdx = 0;
}

static void dx12_CreateCmdPool(void* where)
{
    rz_gfx_cmdpool* cmdPool = (rz_gfx_cmdpool*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&cmdPool->resource.handle), "Invalid cmd pool handle, who is allocating this? ResourceManager should create a valid handle");

    CHECK_HR(ID3D12Device10_CreateCommandAllocator(DX12Device, dx12_util_rz_cmdpool_to_cmd_list_type(cmdPool->resource.desc.cmdpoolDesc.poolType), &IID_ID3D12CommandAllocator, (void**) &cmdPool->dx12.cmdAlloc));
    if (cmdPool->dx12.cmdAlloc == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Command Allocator");
        return;
    }
    cmdPool->type = cmdPool->resource.desc.cmdpoolDesc.poolType;
    TAG_OBJECT(cmdPool->dx12.cmdAlloc, cmdPool->resource.pName);
}

static void dx12_DestroyCmdPool(void* cmdPool)
{
    RAZIX_RHI_ASSERT(cmdPool != NULL, "Command pool is NULL, cannot destroy");
    rz_gfx_cmdpool* cmdPoolPtr = (rz_gfx_cmdpool*) cmdPool;
    if (cmdPoolPtr->dx12.cmdAlloc) {
        ID3D12CommandAllocator_Release(cmdPoolPtr->dx12.cmdAlloc);
        cmdPoolPtr->dx12.cmdAlloc = NULL;
    }
}

static void dx12_CreateCmdBuf(void* where)
{
    rz_gfx_cmdbuf* cmdBuf = (rz_gfx_cmdbuf*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&cmdBuf->resource.handle), "Invalid command buffer handle, who is allocating this? ResourceManager should create a valid handle");
    const rz_gfx_cmdpool* cmdPool = cmdBuf->resource.desc.cmdbufDesc.pool;
    RAZIX_RHI_ASSERT(cmdPool != NULL, "Command buffer must have a valid command pool");
    cmdBuf->dx12.cmdAlloc = cmdBuf->resource.desc.cmdbufDesc.pool->dx12.cmdAlloc;
    RAZIX_RHI_ASSERT(cmdBuf->dx12.cmdAlloc != NULL, "Command buffer must have a valid command allocator");

    CHECK_HR(ID3D12Device10_CreateCommandList(DX12Device, 0, dx12_util_rz_cmdpool_to_cmd_list_type(cmdPool->type), cmdPool->dx12.cmdAlloc, NULL, &IID_ID3D12GraphicsCommandList, (void**) &cmdBuf->dx12.cmdList));
    if (cmdBuf->dx12.cmdList == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Command List");
        return;
    }
    // Immediately close it so that the first use can Reset() safely
    CHECK_HR(ID3D12GraphicsCommandList_Close(cmdBuf->dx12.cmdList));
    TAG_OBJECT(cmdBuf->dx12.cmdList, cmdBuf->resource.pName);
}

static void dx12_DestroyCmdBuf(void* cmdBuf)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer is NULL, cannot destroy");
    rz_gfx_cmdbuf* cmdBufPtr = (rz_gfx_cmdbuf*) cmdBuf;
    if (cmdBufPtr->dx12.cmdList) {
        ID3D12GraphicsCommandList_Release(cmdBufPtr->dx12.cmdList);
        cmdBufPtr->dx12.cmdList = NULL;
    }
}

static void dx12_CreateShader(void* where)
{
    rz_gfx_shader* shader = (rz_gfx_shader*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&shader->resource.handle), "Invalid shader handle, who is allocating this? ResourceManager should create a valid handle");

    // In Direct3D 12, shaders are not standalone objects like in Vulkan.
    // There's no need to create an intermediate shader module.
    // The raw shader bytecode (e.g., .cso or .dxil) is passed directly to the pipeline state during creation.
    // Therefore, this function intentionally does nothing. Actual shader usage happens during PSO creation.

    rz_gfx_shader_desc* desc = &shader->resource.desc.shaderDesc;

    switch (desc->pipelineType) {
        case RZ_GFX_PIPELINE_TYPE_GRAPHICS:
            if (desc->raster.vs.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_VERTEX;
            if (desc->raster.ps.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_PIXEL;
            if (desc->raster.gs.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_GEOMETRY;
            if (desc->raster.tcs.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_TESSELLATION_CONTROL;
            if (desc->raster.tes.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_TESSELLATION_EVALUATION;
            if (desc->raster.task.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_TASK;
            if (desc->raster.mesh.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_MESH;
            break;
        case RZ_GFX_PIPELINE_TYPE_COMPUTE:
            if (desc->compute.cs.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_COMPUTE;
            break;
        case RZ_GFX_PIPELINE_TYPE_RAYTRACING:
            if (desc->raytracing.rgen.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_RAY_GEN;
            if (desc->raytracing.miss.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_RAY_MISS;
            if (desc->raytracing.chit.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_RAY_CLOSEST_HIT;
            if (desc->raytracing.ahit.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_RAY_ANY_HIT;
            if (desc->raytracing.callable.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_RAY_CALLABLE;
        default:
            RAZIX_RHI_ASSERT(false, "Invalid pipeline type for shader!");
            break;
    }
}

static void dx12_DestroyShader(void* shader)
{
    RAZIX_RHI_ASSERT(shader != NULL, "Shader is NULL, cannot destroy");
    rz_gfx_shader* shaderPtr   = (rz_gfx_shader*) shader;
    shaderPtr->shaderStageMask = 0;
    // In D3D12, shaders are not standalone objects, so there's nothing to release here.
}

static void dx12_CreateRootSignature(void* where)
{
    rz_gfx_root_signature* rootSig = (rz_gfx_root_signature*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&rootSig->resource.handle), "Invalid rootsignaure handle, who is allocating this? ResourceManager should create a valid handle");

    const rz_gfx_root_signature_desc* desc = &rootSig->resource.desc.rootSignatureDesc;

    D3D12_ROOT_PARAMETER   rootParams[RAZIX_MAX_DESCRIPTOR_TABLES + RAZIX_MAX_ROOT_CONSTANTS]         = {0};
    D3D12_DESCRIPTOR_RANGE descriptorRanges[RAZIX_MAX_DESCRIPTOR_TABLES][RAZIX_MAX_DESCRIPTOR_RANGES] = {0};

    D3D12_ROOT_SIGNATURE_DESC rootDesc = {0};
    rootDesc.Flags                     = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootDesc.NumParameters             = desc->descriptorTableLayoutsCount + desc->rootConstantCount;
    rootDesc.pParameters               = rootParams;
    // TODO: Use static samplers in future for truly bindless textures
    rootDesc.NumStaticSamplers = 0;
    rootDesc.pStaticSamplers   = NULL;

    for (uint32_t tableIdx = 0; tableIdx < desc->descriptorTableLayoutsCount; tableIdx++) {
        const rz_gfx_descriptor_table_layout* pTableLayouts = &desc->pDescriptorTableLayouts[tableIdx];
        RAZIX_RHI_ASSERT(pTableLayouts != NULL, "Descriptor table cannot be NULL in root signature creation! (Root Signature creation)");

        D3D12_ROOT_PARAMETER* param = &rootParams[tableIdx];
        param->ParameterType        = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

        for (uint32_t rangeIdx = 0; rangeIdx < pTableLayouts->descriptorCount; rangeIdx++) {
            RAZIX_RHI_ASSERT(rangeIdx < RAZIX_MAX_DESCRIPTOR_RANGES, "Too many descriptors in a table! [MAXLIMIT: %d] (Root Signature creation)", RAZIX_MAX_DESCRIPTOR_RANGES);

            const rz_gfx_descriptor* pDescriptor = &pTableLayouts->pDescriptors[rangeIdx];
            RAZIX_RHI_ASSERT(pDescriptor != NULL, "Descriptor cannot be NULL in a descriptor table! (Root Signature creation)");
            RAZIX_RHI_ASSERT(pDescriptor->location.space == pTableLayouts->tableIndex,
                "Descriptor space (%u) does not match table index (%u) in root signature creation! (Root Signature creation)",
                pDescriptor->location.space,
                pTableLayouts->tableIndex);

            D3D12_DESCRIPTOR_RANGE* range            = &descriptorRanges[tableIdx][rangeIdx];
            range->RangeType                         = dx12_util_descriptor_type_to_range_type(pDescriptor->type);
            range->NumDescriptors                    = pDescriptor->memberCount;
            range->BaseShaderRegister                = pDescriptor->location.binding;
            range->RegisterSpace                     = pTableLayouts->tableIndex;
            range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        }

        param->DescriptorTable.NumDescriptorRanges = pTableLayouts->descriptorCount;
        param->DescriptorTable.pDescriptorRanges   = descriptorRanges[tableIdx];
    }

    for (uint32_t i = 0; i < desc->rootConstantCount; i++) {
        const rz_gfx_root_constant_desc* pRootConstantDesc = &desc->pRootConstantsDesc[i];
        RAZIX_RHI_ASSERT(pRootConstantDesc != NULL, "Root constant cannot be NULL in root signature creation! (Root Signature creation)");

        D3D12_ROOT_PARAMETER* param     = &rootParams[desc->descriptorTableLayoutsCount + i];
        param->ParameterType            = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        param->Constants.Num32BitValues = pRootConstantDesc->sizeInBytes >> 2;
        param->Constants.ShaderRegister = pRootConstantDesc->location.binding;
        param->Constants.RegisterSpace  = pRootConstantDesc->location.space;
    }

    ID3DBlob* signatureBlob = NULL;
    ID3DBlob* errorBlob     = NULL;
    HRESULT   hr            = D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr)) {
        RAZIX_RHI_LOG_ERROR("Failed to serialize root signature: %s", errorBlob ? (const char*) ID3D10Blob_GetBufferPointer(errorBlob) : "Unknown error");
        if (errorBlob) ID3D10Blob_Release(errorBlob);
        return;
    }
    RAZIX_RHI_LOG_INFO("Root signature serialized successfully");

    CHECK_HR(ID3D12Device10_CreateRootSignature(DX12Device, 0, ID3D10Blob_GetBufferPointer(signatureBlob), ID3D10Blob_GetBufferSize(signatureBlob), &IID_ID3D12RootSignature, (void**) &rootSig->dx12.rootSig));
    if (rootSig->dx12.rootSig == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Root Signature");
        ID3D10Blob_Release(signatureBlob);
        return;
    }

    RAZIX_RHI_LOG_INFO("D3D12 Root Signature created successfully");
    TAG_OBJECT(rootSig->dx12.rootSig, rootSig->resource.pName);
}

static void dx12_DestroyRootSignature(void* ptr)
{
    RAZIX_RHI_ASSERT(ptr != NULL, "Root signature is NULL, cannot destroy");
    rz_gfx_root_signature* rootSig = (rz_gfx_root_signature*) ptr;

    if (rootSig->dx12.rootSig) {
        ID3D12RootSignature_Release(rootSig->dx12.rootSig);
        rootSig->dx12.rootSig = NULL;
    }
}

static void dx12_CreateGraphicsPipeline(rz_gfx_pipeline* pso)
{
    const rz_gfx_pipeline_desc*  pPsoDesc    = &pso->resource.desc.pipelineDesc;
    const rz_gfx_shader*         pShader     = pso->resource.desc.pipelineDesc.pShader;
    const rz_gfx_shader_desc*    pShaderDesc = &pShader->resource.desc.shaderDesc;
    const rz_gfx_root_signature* pRootSig    = pso->resource.desc.pipelineDesc.pRootSig;
    RAZIX_RHI_ASSERT(pShader != NULL, "Pipeline must have a valid shader! (Pipeline creation)");
    RAZIX_RHI_ASSERT(pRootSig != NULL, "Pipeline must have a valid root signature! (Pipeline creation)");
    RAZIX_RHI_ASSERT(pShaderDesc->pipelineType == RZ_GFX_PIPELINE_TYPE_GRAPHICS, "Shader must be a graphics shader for this pipeline type! (Pipeline creation)");

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {0};
    desc.NodeMask                           = 0;    // Single GPU, no multi-GPU support
    desc.Flags                              = D3D12_PIPELINE_STATE_FLAG_NONE;
    desc.pRootSignature                     = pRootSig->dx12.rootSig;
    RAZIX_RHI_ASSERT(desc.pRootSignature != NULL, "Root signature cannot be NULL in pipeline creation! (Pipeline creation)");

    //----------------------------
    // Shaders
    //----------------------------
    desc.VS = dx12_util_shader_bytecode_to_d3d12_shader(&pShaderDesc->raster.vs);
    desc.PS = dx12_util_shader_bytecode_to_d3d12_shader(&pShaderDesc->raster.ps);
    desc.GS = dx12_util_shader_bytecode_to_d3d12_shader(&pShaderDesc->raster.gs);
    desc.HS = dx12_util_shader_bytecode_to_d3d12_shader(&pShaderDesc->raster.tes);
    desc.DS = dx12_util_shader_bytecode_to_d3d12_shader(&pShaderDesc->raster.tcs);

    //----------------------------
    // Input Assembly Stage
    //----------------------------
    D3D12_INPUT_ELEMENT_DESC* pInputElementDescs = alloca(pShaderDesc->elementsCount * sizeof(D3D12_INPUT_ELEMENT_DESC));

    for (uint32_t i = 0; i < pShaderDesc->elementsCount; i++) {
        rz_gfx_input_element* elem = &pShaderDesc->pElements[i];
        RAZIX_RHI_ASSERT(elem->format != RZ_GFX_FORMAT_UNDEFINED, "Input element format cannot be undefined");
        RAZIX_RHI_ASSERT(i < RAZIX_MAX_VERTEX_ATTRIBUTES, "Input element location exceeds maximum vertex attributes");

        D3D12_INPUT_ELEMENT_DESC* dxElement = &pInputElementDescs[i];
        dxElement->SemanticName             = (LPCSTR) elem->pSemanticName;
        dxElement->SemanticIndex            = elem->semanticIndex;
        dxElement->Format                   = dx12_util_rz_gfx_format_to_dxgi_format(elem->format);

        if (pPsoDesc->inputLayoutMode == RZ_GFX_INPUT_LAYOUT_AOS) {
            dxElement->InputSlot         = 0;                          // single buffer
            dxElement->AlignedByteOffset = elem->alignedByteOffset;    // offset into struct
        } else {                                                       // SOA
            dxElement->InputSlot         = i;                          // each attribute in own slot
            dxElement->AlignedByteOffset = 0;                          // attribute buffer starts at element
        }

        dxElement->InputSlotClass       = (elem->inputClass == RZ_GFX_INPUT_CLASS_PER_VERTEX)
                                              ? D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA
                                              : D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
        dxElement->InstanceDataStepRate = elem->instanceStepRate;

        printf("Element %d: SemanticName = %s, Format = %d, Offset = %d\n",
            i,
            elem->pSemanticName,
            elem->format,
            elem->alignedByteOffset);
    }
    // stride is set during VeretxBuffers binding and is decided by the user, unlike in VK it must be given at runtime, so yeah
    D3D12_INPUT_LAYOUT_DESC input_layout = {0};
    input_layout.NumElements             = pShaderDesc->elementsCount;
    input_layout.pInputElementDescs      = pInputElementDescs;
    desc.InputLayout                     = input_layout;

    //----------------------------
    // Primitive Topology
    //----------------------------
    D3D12_PRIMITIVE_TOPOLOGY_TYPE prim_topology = {0};
    prim_topology                               = dx12_util_draw_type_to_topology(pPsoDesc->drawType);
    desc.PrimitiveTopologyType                  = prim_topology;    // this is just the category of primitives no the exact formation style of primitives
    // Just translate and cache it to set during IA on command list, this is a dynamic state in DX12 unlike Vulkan
    pso->dx12.topology = dx12_util_draw_type_to_d3dtopology(pPsoDesc->drawType);

    //----------------------------
    // Rasterizer Stage
    //----------------------------
    D3D12_RASTERIZER_DESC rasterizer = {0};
    rasterizer.FillMode              = dx12_util_polygon_mode_to_fill_mode(pPsoDesc->polygonMode);
    rasterizer.CullMode              = dx12_util_cull_mode_translate(pPsoDesc->cullMode);
    rasterizer.FrontCounterClockwise = TRUE;
    rasterizer.DepthClipEnable       = FALSE;
    rasterizer.MultisampleEnable     = FALSE;
    desc.RasterizerState             = rasterizer;

    //----------------------------
    // Render/DS Targets & Formats
    //----------------------------
    desc.NumRenderTargets = pso->resource.desc.pipelineDesc.renderTargetCount;
    for (uint32_t i = 0; i < pso->resource.desc.pipelineDesc.renderTargetCount; ++i) {
        desc.RTVFormats[i] = dx12_util_rz_gfx_format_to_dxgi_format(pso->resource.desc.pipelineDesc.renderTargetFormats[i]);
    }

    if (pso->resource.desc.pipelineDesc.depthTestEnabled || pso->resource.desc.pipelineDesc.depthWriteEnabled || pso->resource.desc.pipelineDesc.stencilTestEnabled) {
        desc.DSVFormat = dx12_util_rz_gfx_format_to_dxgi_format(pso->resource.desc.pipelineDesc.depthStencilFormat);
    } else {
        desc.DSVFormat = DXGI_FORMAT_UNKNOWN;    // No depth-stencil
    }

    //----------------------------
    // Color Blend State
    //----------------------------
    D3D12_BLEND_DESC blendState       = {0};
    blendState.AlphaToCoverageEnable  = FALSE;
    blendState.IndependentBlendEnable = FALSE;

    D3D12_RENDER_TARGET_BLEND_DESC rtBlendDesc = {0};
    rtBlendDesc.BlendEnable                    = pPsoDesc->blendEnabled;
    if (pPsoDesc->useBlendPreset) {
        rtBlendDesc = dx12_util_blend_preset(pPsoDesc->blendEnabled);
    } else {
        rtBlendDesc.SrcBlend              = dx12_util_blend_factor(pPsoDesc->srcColorBlendFactor);
        rtBlendDesc.DestBlend             = dx12_util_blend_factor(pPsoDesc->dstColorBlendFactor);
        rtBlendDesc.BlendOp               = dx12_util_blend_op(pPsoDesc->colorBlendOp);
        rtBlendDesc.SrcBlendAlpha         = dx12_util_blend_factor(pPsoDesc->srcAlphaBlendFactor);
        rtBlendDesc.DestBlendAlpha        = dx12_util_blend_factor(pPsoDesc->dstAlphaBlendFactor);
        rtBlendDesc.BlendOpAlpha          = dx12_util_blend_op(pPsoDesc->alphaBlendOp);
        rtBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    }

    // TODO: Allow blend state customization per RT
    for (uint32_t i = 0; i < desc.NumRenderTargets; i++)
        blendState.RenderTarget[i] = rtBlendDesc;
    desc.BlendState = blendState;

    //----------------------------
    // Depth Stencil Stage
    //----------------------------
    D3D12_DEPTH_STENCIL_DESC depth = {0};
    depth.DepthEnable              = pPsoDesc->depthTestEnabled;
    depth.DepthWriteMask           = pPsoDesc->depthWriteEnabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    depth.DepthFunc                = dx12_util_compare_func(pPsoDesc->depthCompareOp);
    depth.StencilEnable            = FALSE;
    depth.StencilReadMask          = D3D12_DEFAULT_STENCIL_READ_MASK;
    depth.StencilWriteMask         = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    depth.FrontFace                = (D3D12_DEPTH_STENCILOP_DESC) {
                       .StencilFailOp      = D3D12_STENCIL_OP_KEEP,
                       .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
                       .StencilPassOp      = D3D12_STENCIL_OP_KEEP,
                       .StencilFunc        = D3D12_COMPARISON_FUNC_ALWAYS};
    depth.BackFace         = depth.FrontFace;
    desc.DepthStencilState = depth;

    //----------------------------
    // Multi sample State (MSAA)
    //----------------------------
    desc.SampleDesc.Count   = 1;
    desc.SampleDesc.Quality = 0;
    desc.SampleMask         = UINT_MAX;    // No sample mask

    // Create the pipeline state object
    HRESULT hr = ID3D12Device10_CreateGraphicsPipelineState(DX12Device, &desc, &IID_ID3D12PipelineState, (void**) &pso->dx12.pso);
    if (FAILED(hr)) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Pipeline State Object (PSO): 0x%08X", hr);
        return;
    }
    RAZIX_RHI_LOG_INFO("D3D12 Pipeline State Object (PSO) created successfully");
    TAG_OBJECT(pso->dx12.pso, pso->resource.pName);
}

static void dx12_CreateComputePipeline(rz_gfx_pipeline* pso)
{
    const rz_gfx_pipeline_desc*  pPsoDesc    = &pso->resource.desc.pipelineDesc;
    const rz_gfx_shader*         pShader     = pso->resource.desc.pipelineDesc.pShader;
    const rz_gfx_shader_desc*    pShaderDesc = &pShader->resource.desc.shaderDesc;
    const rz_gfx_root_signature* pRootSig    = pso->resource.desc.pipelineDesc.pRootSig;
    RAZIX_RHI_ASSERT(pShader != NULL, "Pipeline must have a valid shader! (Pipeline creation)");
    RAZIX_RHI_ASSERT(pRootSig != NULL, "Pipeline must have a valid root signature! (Pipeline creation)");
    RAZIX_RHI_ASSERT(pShaderDesc->pipelineType == RZ_GFX_PIPELINE_TYPE_COMPUTE, "Shader must be a compute shader for this pipeline type! (Pipeline creation)");

    D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {0};
    desc.NodeMask                          = 0;    // Single GPU, no multi-GPU support
    desc.Flags                             = D3D12_PIPELINE_STATE_FLAG_NONE;
    desc.pRootSignature                    = pRootSig->dx12.rootSig;
    RAZIX_RHI_ASSERT(desc.pRootSignature != NULL, "Root signature cannot be NULL in pipeline creation! (Pipeline creation)");

    //----------------------------
    // Shaders
    //----------------------------
    desc.CS = dx12_util_shader_bytecode_to_d3d12_shader(&pShaderDesc->compute.cs);

    // Create the pipeline state object
    HRESULT hr = ID3D12Device10_CreateComputePipelineState(DX12Device, &desc, &IID_ID3D12PipelineState, (void**) &pso->dx12.pso);
    if (FAILED(hr)) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Pipeline State Object (PSO): 0x%08X", hr);
        return;
    }
    RAZIX_RHI_LOG_INFO("D3D12 Pipeline State Object (PSO) created successfully");
    TAG_OBJECT(pso->dx12.pso, pso->resource.pName);
}

static void dx12_CreatePipeline(void* pipeline)
{
    rz_gfx_pipeline* pso = (rz_gfx_pipeline*) pipeline;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&pso->resource.handle), "Invalid pipeline handle, who is allocating this? ResourceManager should create a valid handle");

    if (pso->resource.desc.pipelineDesc.type == RZ_GFX_PIPELINE_TYPE_GRAPHICS)
        dx12_CreateGraphicsPipeline(pso);
    else
        dx12_CreateComputePipeline(pso);
}

static void dx12_DestroyPipeline(void* pipeline)
{
    RAZIX_RHI_ASSERT(pipeline != NULL, "Pipeline is NULL, cannot destroy");
    rz_gfx_pipeline* pso = (rz_gfx_pipeline*) pipeline;

    if (pso->dx12.pso) {
        ID3D12PipelineState_Release(pso->dx12.pso);
        pso->dx12.pso = NULL;
    }
}

static void dx12_CreateTexture(void* where)
{
    rz_gfx_texture* texture = (rz_gfx_texture*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&texture->resource.handle), "Invalid texture handle, who is allocating this? ResourceManager should create a valid handle");
    rz_gfx_texture_desc* desc = &texture->resource.desc.textureDesc;
    RAZIX_RHI_ASSERT(desc != NULL, "Texture descriptor cannot be NULL");
    RAZIX_RHI_ASSERT(desc->width > 0 && desc->height > 0 && desc->depth > 0, "Texture dimensions must be greater than zero");

    // Maintain a second copy of hints...Ahhh...
    texture->resource.viewHints = desc->resourceHints;

    D3D12_RESOURCE_DESC resDesc = {0};
    resDesc.Dimension           = dx12_util_translate_texture_dimension(desc->textureType);
    resDesc.Width               = desc->width;
    resDesc.Height              = desc->height;
    resDesc.DepthOrArraySize    = desc->depth;
    resDesc.MipLevels           = desc->mipLevels;
    resDesc.Format              = dx12_util_rz_gfx_format_to_dxgi_format(desc->format);
    resDesc.SampleDesc.Count    = 1;
    resDesc.Layout              = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;

    D3D12_CLEAR_VALUE optClear    = {0};
    optClear.Format               = resDesc.Format;
    optClear.Color[0]             = 0.0f;
    optClear.Color[1]             = 0.0f;
    optClear.Color[2]             = 0.0f;
    optClear.Color[3]             = 0.0f;
    optClear.DepthStencil.Depth   = 1.0f;
    optClear.DepthStencil.Stencil = 0;

    texture->resource.currentState = RZ_GFX_RESOURCE_STATE_COMMON;

    bool isRtvDsv = (desc->resourceHints & (RZ_GFX_RESOURCE_VIEW_FLAG_RTV | RZ_GFX_RESOURCE_VIEW_FLAG_DSV)) != 0;
    // Set resource flags
    if ((desc->resourceHints & RZ_GFX_RESOURCE_VIEW_FLAG_UAV) == RZ_GFX_RESOURCE_VIEW_FLAG_UAV)
        resDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    else if ((desc->resourceHints & RZ_GFX_RESOURCE_VIEW_FLAG_RTV) == RZ_GFX_RESOURCE_VIEW_FLAG_RTV)
        resDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    else if ((desc->resourceHints & RZ_GFX_RESOURCE_VIEW_FLAG_DSV) == RZ_GFX_RESOURCE_VIEW_FLAG_DSV) {
        resDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        texture->resource.currentState = RZ_GFX_RESOURCE_STATE_DEPTH_WRITE;
    }

    // Create resource with memory backing
    D3D12_HEAP_PROPERTIES heapProps = {0};
    heapProps.Type                  = D3D12_HEAP_TYPE_DEFAULT;
    heapProps.CPUPageProperty       = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference  = D3D12_MEMORY_POOL_UNKNOWN;

    // Create the texture resource
    HRESULT hr = ID3D12Device10_CreateCommittedResource(DX12Device, &heapProps, D3D12_HEAP_FLAG_NONE, &resDesc, dx12_util_res_state_translate(texture->resource.currentState), isRtvDsv ? &optClear : NULL, &IID_ID3D12Resource, &texture->dx12.resource);
    if (FAILED(hr)) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Texture2D: 0x%08X", hr);
        return;
    }
    RAZIX_RHI_LOG_INFO("D3D12 Texture2D created successfully");
    TAG_OBJECT(texture->dx12.resource, texture->resource.pName);

    // Upload pixel data if provided
    if (desc->pPixelData != NULL) {
        RAZIX_RHI_LOG_INFO("Uploading pixel data for texture");
        dx12_util_upload_pixel_Data(texture, desc);
    }
}

static void dx12_DestroyTexture(void* texture)
{
    RAZIX_RHI_ASSERT(texture != NULL, "Texture is NULL, cannot destroy");
    rz_gfx_texture* tex = (rz_gfx_texture*) texture;
    if (tex->dx12.resource) {
        ID3D12Resource_Release(tex->dx12.resource);
        tex->dx12.resource = NULL;
    }
}

static void dx12_CreateSampler(void* where)
{
    rz_gfx_sampler* sampler = (rz_gfx_sampler*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&sampler->resource.handle), "Invalid sampler handle, who is allocating this? ResourceManager should create a valid handle");
    // This is empty we don't need to create anything for samplers in D3D12
    // Samplers are created during descriptor heap creation and bound to the pipeline state.
}

static void dx12_DestroySampler(void* sampler)
{
    RAZIX_RHI_ASSERT(sampler != NULL, "Sampler is NULL, cannot destroy");
    rz_gfx_sampler* sam = (rz_gfx_sampler*) sampler;
    // In D3D12, samplers are not standalone objects, so there's nothing to release here.
    // They are managed by the descriptor heaps and bound to the pipeline state.
    // Destroying the descriptor heap will clean up the samplers.
}

static void dx12_CreateBuffer(void* where)
{
    rz_gfx_buffer* buffer = (rz_gfx_buffer*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&buffer->resource.handle), "Invalid buffer handle, who is allocating this? ResourceManager should create a valid handle");
    rz_gfx_buffer_desc* desc = &buffer->resource.desc.bufferDesc;
    RAZIX_RHI_ASSERT(desc != NULL, "Buffer descriptor cannot be NULL");
    RAZIX_RHI_ASSERT(desc->sizeInBytes > 0, "Buffer size must be greater than zero");

    // Maintain a second copy of hints...Ahhh...
    buffer->resource.viewHints = desc->resourceHints;

#ifdef RAZIX_DEBUG
    if (desc->type == RZ_GFX_BUFFER_TYPE_STRUCTURED || desc->type == RZ_GFX_BUFFER_TYPE_RW_STRUCTURED) {
        RAZIX_RHI_ASSERT(desc->stride > 0, "Structured buffer must have a valid stride");
        RAZIX_RHI_ASSERT((desc->sizeInBytes % desc->stride) == 0, "Structured buffer size must be a multiple of the stride");
    }
#endif

    // create constant buffers aligned to 256 bytes
    bool isConstantBuffer = desc->type == RZ_GFX_BUFFER_TYPE_CONSTANT;
    if (isConstantBuffer) {
        if (desc->sizeInBytes % RAZIX_CONSTANT_BUFFER_MIN_ALIGNMENT != 0) {
            desc->sizeInBytes = RAZIX_RHI_ALIGN(desc->sizeInBytes, RAZIX_CONSTANT_BUFFER_MIN_ALIGNMENT);
            RAZIX_RHI_LOG_WARN("Buffer size rounded up to %u bytes to meet constant buffer alignment requirements of %d bytes", desc->sizeInBytes, RAZIX_CONSTANT_BUFFER_MIN_ALIGNMENT);
        }

        if (desc->usage == RZ_GFX_BUFFER_USAGE_TYPE_STATIC) {
            RAZIX_RHI_LOG_WARN("Static usage is not recommended for constant buffers, consider using DYNAMIC or PERSISTENT_STREAM usage types for better data upload mechanism or use push constant for static data/textures");
        }
    }

    D3D12_RESOURCE_DESC resDesc = {0};
    resDesc.Dimension           = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Alignment           = 0;    //isConstantBuffer ? RAZIX_CONSTANT_BUFFER_MIN_ALIGNMENT : 0;    // 256 for constant buffers, 0 for others
    resDesc.Width               = desc->sizeInBytes;
    resDesc.Height              = 1;
    resDesc.DepthOrArraySize    = 1;
    resDesc.MipLevels           = 1;
    resDesc.Format              = DXGI_FORMAT_UNKNOWN;    // TODO: Deduce this for Structured buffers here on in D3D12_BUFFER_DESC? ex. dx12_util_rz_gfx_format_to_dxgi_format(desc->format)
    resDesc.SampleDesc.Count    = 1;
    resDesc.Layout              = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;    // as fed by users
    resDesc.Flags               = D3D12_RESOURCE_FLAG_NONE;

    if ((desc->resourceHints & RZ_GFX_RESOURCE_VIEW_FLAG_UAV) || (desc->type == RZ_GFX_BUFFER_TYPE_INDIRECT_ARGS))
        resDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    // Create resource with memory backing
    D3D12_HEAP_PROPERTIES heapProps = {0};
    // TODO: Choose upload for other types if CPU access is needed, like streaming vertex/index buffers
    bool useUploadHeapType         = desc->usage == RZ_GFX_BUFFER_USAGE_TYPE_DYNAMIC || desc->usage == RZ_GFX_BUFFER_USAGE_TYPE_STAGING || desc->usage == RZ_GFX_BUFFER_USAGE_TYPE_PERSISTENT_STREAM;
    heapProps.Type                 = useUploadHeapType ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;
    heapProps.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    buffer->resource.currentState = RZ_GFX_RESOURCE_STATE_COMMON;
    if (useUploadHeapType)
        buffer->resource.currentState = RZ_GFX_RESOURCE_STATE_GENERIC_READ;

    // Create the buffer resource
    HRESULT hr = ID3D12Device10_CreateCommittedResource(DX12Device, &heapProps, D3D12_HEAP_FLAG_NONE, &resDesc, dx12_util_res_state_translate(buffer->resource.currentState), NULL, &IID_ID3D12Resource, &buffer->dx12.resource);
    if (FAILED(hr)) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Buffer: 0x%08X", hr);
        return;
    }

    if (desc->pInitData != NULL) {
        RAZIX_RHI_LOG_INFO("Uploading initial data for buffer");

        if (((desc->type & RZ_GFX_BUFFER_TYPE_CONSTANT) == RZ_GFX_BUFFER_TYPE_CONSTANT) && useUploadHeapType) {
            void*   mappedData = NULL;
            HRESULT hr         = ID3D12Resource_Map(buffer->dx12.resource, 0, NULL, &mappedData);
            if (FAILED(hr) || mappedData == NULL) {
                RAZIX_RHI_LOG_ERROR("Failed to map constant buffer memory for initial data upload: 0x%08X", hr);
                return;
            }
            memcpy(mappedData, desc->pInitData, desc->sizeInBytes);
            ID3D12Resource_Unmap(buffer->dx12.resource, 0, NULL);
            RAZIX_RHI_LOG_INFO("Constant buffer initial data uploaded via direct mapping");
        } else {
            // Use staging buffer method for default heap buffers (VB/IB/Indirect/etc.)
            dx12_util_upload_buffer_data(buffer, desc);
        }
    }

    TAG_OBJECT(buffer->dx12.resource, buffer->resource.pName);
}

static void dx12_DestroyBuffer(void* buffer)
{
    RAZIX_RHI_ASSERT(buffer != NULL, "Buffer is NULL, cannot destroy");
    rz_gfx_buffer* buf = (rz_gfx_buffer*) buffer;
    if (buf->dx12.resource) {
        ID3D12Resource_Release(buf->dx12.resource);
        buf->dx12.resource = NULL;
    }
}

static void dx12_CreateDescriptorHeap(void* where)
{
    rz_gfx_descriptor_heap* heap = (rz_gfx_descriptor_heap*) where;
    RAZIX_RHI_ASSERT(heap != NULL, "Descriptor heap cannot be NULL");
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&heap->resource.handle), "Invalid descriptor heap handle, who is allocating this? ResourceManager should create a valid handle");
    rz_gfx_descriptor_heap_desc* desc = &heap->resource.desc.descriptorHeapDesc;
    RAZIX_RHI_ASSERT(desc != NULL, "Descriptor heap descriptor cannot be NULL");

    D3D12_DESCRIPTOR_HEAP_DESC d3d12Desc = {0};
    d3d12Desc.Type                       = dx12_util_descriptor_heap_type(desc->heapType);
    d3d12Desc.NumDescriptors             = desc->descriptorCount;
    if (desc->flags & RZ_GFX_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
        d3d12Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    } else {
        d3d12Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    }
    d3d12Desc.NodeMask = 0;    // Single GPU, no multi-GPU support
    HRESULT hr         = ID3D12Device10_CreateDescriptorHeap(DX12Device, &d3d12Desc, &IID_ID3D12DescriptorHeap, (void**) &heap->dx12.heap);
    if (FAILED(hr)) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Descriptor Heap: 0x%08X", hr);
        return;
    }
    RAZIX_RHI_LOG_INFO("D3D12 Descriptor Heap created successfully");
    TAG_OBJECT(heap->dx12.heap, heap->resource.pName);

    // Cache the descriptor size
    heap->dx12.descriptorSize = ID3D12Device10_GetDescriptorHandleIncrementSize(DX12Device, d3d12Desc.Type);

    // cache the CPU/GPU offsets
    ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(heap->dx12.heap, &heap->dx12.heapStart.cpu);
    if (desc->flags & RZ_GFX_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
        // If the heap is shader visible, we need to get the GPU handle as well
        ID3D12DescriptorHeap_GetGPUDescriptorHandleForHeapStart(heap->dx12.heap, &heap->dx12.heapStart.gpu);
    }

    RAZIX_RHI_LOG_INFO("Descriptor memory backing (ringbuffer): %d", desc->flags & RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_RINGBUFFER);
    RAZIX_RHI_LOG_INFO("Descriptor memory backing (freelist): %d", desc->flags & RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_FREELIST);

    // Create backing for allocation based on free list vs ring buffer
    if ((desc->flags & RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_FREELIST) == RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_FREELIST) {
        dx12_create_descriptor_freelist_allocator(heap);
    } else if ((desc->flags & RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_RINGBUFFER) == RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_RINGBUFFER) {
        heap->ringBufferHead = 0;
        heap->ringBufferTail = 0;
        heap->isFull         = false;
    } else {
        RAZIX_RHI_LOG_ERROR("Descriptor heap alloc backing must be of type FREE_LIST or RING_BUFFER for allocation");
        RAZIX_RHI_ABORT();
    }
}

static void dx12_DestroyDescriptorHeap(void* heap)
{
    RAZIX_RHI_ASSERT(heap != NULL, "Descriptor heap is NULL, cannot destroy");
    rz_gfx_descriptor_heap* descHeap = (rz_gfx_descriptor_heap*) heap;
    if (descHeap->dx12.heap) {
        ID3D12DescriptorHeap_Release(descHeap->dx12.heap);
        descHeap->dx12.heap = NULL;

        descHeap->dx12.descriptorSize    = 0;
        descHeap->dx12.heapStart.gpu.ptr = 0;
        descHeap->dx12.heapStart.cpu.ptr = 0;

        rz_gfx_descriptor_heap_desc* desc = &descHeap->resource.desc.descriptorHeapDesc;

        if (descHeap->freeListAllocator && (desc->flags & RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_FREELIST))
            dx12_destroy_descriptor_freelist_allocator(descHeap);
        else if (desc->flags & RZ_GFX_DESCRIPTOR_HEAP_FLAG_DESCRIPTOR_ALLOC_RINGBUFFER) {
            descHeap->ringBufferHead = 0;
            descHeap->ringBufferTail = 0;
            descHeap->isFull         = false;
        }
    }
}

static void dx12_CreateResourceView(void* where)
{
    rz_gfx_resource_view* pView = (rz_gfx_resource_view*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&pView->resource.handle), "Invalid resource view handle, who is allocating this? ResourceManager should create a valid handle");
    rz_gfx_resource_view_desc* pViewDesc = &pView->resource.desc.resourceViewDesc;
    RAZIX_RHI_ASSERT(pViewDesc != NULL, "Resource view descriptor cannot be NULL");
    RAZIX_RHI_ASSERT(pViewDesc->descriptorType != RZ_GFX_DESCRIPTOR_TYPE_NONE, "Resource view descriptor type cannot be none");

    // Create the resource view based on the type
    if (rzRHI_IsDescriptorTypeTexture(pViewDesc->descriptorType)) {
        pView->dx12 = dx12_create_texture_view(&pViewDesc->textureViewDesc, pViewDesc->descriptorType);
    } else if (rzRHI_IsDescriptorTypeBuffer(pViewDesc->descriptorType)) {
        pView->dx12 = dx12_create_buffer_view(&pViewDesc->bufferViewDesc, pViewDesc->descriptorType);
    } else if (pViewDesc->descriptorType == RZ_GFX_DESCRIPTOR_TYPE_SAMPLER) {
        pView->dx12 = dx12_create_sampler_view(&pViewDesc->samplerViewDesc);
    } else {
        RAZIX_RHI_LOG_ERROR("Unsupported resource view descriptor type: %d", pViewDesc->descriptorType);
        RAZIX_RHI_ABORT();
        return;
    }

    // Place them in heap immediately if it's a RTV or DSV since they don't make sense in tables
    // Other descriptor types will be deferred to table creation to place in heaps appropriately
    // Additional view hints checks are already done so just use this for branching
    if (pViewDesc->descriptorType == RZ_GFX_DESCRIPTOR_TYPE_RENDER_TEXTURE) {
        RAZIX_RHI_ASSERT(pViewDesc->pRtvDsvHeap != NULL, "RTV/DSV Resource view cannot be created with NULL RTV/DSV heap, please provide valid heap to allocated RTV/DSV from.");
        dx12_descriptor_handles rtvHandle = dx12_descriptor_allocate_handle(pViewDesc->pRtvDsvHeap, 1);
        ID3D12Device_CreateRenderTargetView(DX12Device, pViewDesc->textureViewDesc.pTexture->dx12.resource, &pView->dx12.rtvDesc, rtvHandle.cpu);
        // Cache the handle
        pView->dx12.rtv = rtvHandle;
    } else if (pViewDesc->descriptorType == RZ_GFX_DESCRIPTOR_TYPE_DEPTH_STENCIL_TEXTURE) {
        RAZIX_RHI_ASSERT(pViewDesc->pRtvDsvHeap != NULL, "RTV/DSV Resource view cannot be created with NULL RTV/DSV heap, please provide valid heap to allocated RTV/DSV from.");
        dx12_descriptor_handles dsvHandle = dx12_descriptor_allocate_handle(pViewDesc->pRtvDsvHeap, 1);
        ID3D12Device_CreateDepthStencilView(DX12Device, pViewDesc->textureViewDesc.pTexture->dx12.resource, &pView->dx12.dsvDesc, dsvHandle.cpu);
        // Cache the handle
        pView->dx12.dsv = dsvHandle;
    }
}

static void dx12_DestroyResourceView(void* where)
{
    RAZIX_RHI_ASSERT(where != NULL, "Resource view is NULL, cannot destroy");
    rz_gfx_resource_view* pView = (rz_gfx_resource_view*) where;
    // Nothing to do here, views are not standalone objects in D3D12
    // They are managed by the descriptor heaps
    pView->dx12 = (dx12_resview) {0};

    // Except for RTV and DSV we free their slots in the heap as the scope is not tied to tables
    rz_gfx_resource_view_desc* pViewDesc = &pView->resource.desc.resourceViewDesc;
    RAZIX_RHI_ASSERT(pViewDesc != NULL, "Resource view descriptor cannot be NULL");
    if (pViewDesc->descriptorType == RZ_GFX_DESCRIPTOR_TYPE_RENDER_TEXTURE)
        dx12_descriptor_free_handle(pView->resource.desc.resourceViewDesc.pRtvDsvHeap, pView->dx12.rtv, 1);
    else if (pViewDesc->descriptorType == RZ_GFX_DESCRIPTOR_TYPE_DEPTH_STENCIL_TEXTURE)
        dx12_descriptor_free_handle(pView->resource.desc.resourceViewDesc.pRtvDsvHeap, pView->dx12.dsv, 1);
}

static void dx12_CreateDescriptorTable(void* where)
{
    rz_gfx_descriptor_table* pTable = (rz_gfx_descriptor_table*) where;
    RAZIX_RHI_ASSERT(rz_handle_is_valid(&pTable->resource.handle), "Invalid table handle, who is allocating this? ResourceManager should create a valid handle");

    rz_gfx_descriptor_table_desc* pDesc = &pTable->resource.desc.descriptorTableDesc;
    RAZIX_RHI_ASSERT(pDesc != NULL, "Descriptor table descriptor cannot be NULL");
    RAZIX_RHI_ASSERT(pDesc->descriptorCount > 0, "Descriptor table should have atleast 1 descriptor");
    RAZIX_RHI_ASSERT(pDesc->pDescriptors != NULL, "Descriptor table cannot have NULL resource views");
    RAZIX_RHI_ASSERT(pDesc->pHeap != NULL, "Descriptor tables needs a heap to create the table");

    pTable->dx12.heapOffset = dx12_descriptor_allocate_handle(pDesc->pHeap, pDesc->descriptorCount);
}

static void dx12_DestroyDescriptorTable(void* where)
{
    RAZIX_RHI_ASSERT(where != NULL, "Descriptor table is NULL, cannot destroy");
    rz_gfx_descriptor_table* pTable = (rz_gfx_descriptor_table*) where;

    rz_gfx_descriptor_table_desc* desc = &pTable->resource.desc.descriptorTableDesc;
    RAZIX_RHI_ASSERT(desc != NULL, "Descriptor table descriptor cannot be NULL");
    RAZIX_RHI_ASSERT(desc->pHeap != NULL, "Descriptor tables needs a heap to create the table");

    if (desc)
        dx12_descriptor_free_handle(desc->pHeap, pTable->dx12.heapOffset, desc->descriptorCount);
}

//---------------------------------------------------------------------------------------------
// RHI

static void dx12_AcquireImage(rz_gfx_swapchain* sc, const rz_gfx_syncobj* presentSignalSyncobj)
{
    RAZIX_RHI_ASSERT(sc != NULL, "Swapchain cannot be NULL");
    RAZIX_RHI_ASSERT(presentSignalSyncobj != NULL, "Present signal sync object cannot be NULL");

    // used in vulkan by the acquire next image function
    // VkQueueSubmit waits on this semaphore until the image is ready before rendering
    // Then it signals the render complete semaphore when rendering is done for that swapchain image
    (void) presentSignalSyncobj;
    sc->currBackBufferIdx = IDXGISwapChain4_GetCurrentBackBufferIndex(sc->dx12.swapchain4);
}

static void dx12_WaitOnPrevCmds(const rz_gfx_syncobj* frameSyncobj)
{
    RAZIX_RHI_ASSERT(frameSyncobj != NULL, "Frame sync object cannot be NULL");

    rz_gfx_syncpoint completed = ID3D12Fence_GetCompletedValue(frameSyncobj->dx12.fence);

    if (completed < frameSyncobj->waitSyncpoint) {
        // Set the fence event and check for failure
        HRESULT hr = ID3D12Fence_SetEventOnCompletion(frameSyncobj->dx12.fence, frameSyncobj->waitSyncpoint, frameSyncobj->dx12.eventHandle);
        if (FAILED(hr)) {
            RAZIX_RHI_LOG_ERROR("[WAIT ERR] SetEventOnCompletion(%llu) failed -> 0x%08X", frameSyncobj->waitSyncpoint, hr);
        }

        // Wait for the event and log errors only
        DWORD result = WaitForSingleObject(frameSyncobj->dx12.eventHandle, INFINITE);
        if (result != WAIT_OBJECT_0) {
            RAZIX_RHI_LOG_ERROR("[WAIT ERR] WaitForSingleObject -> %s",
                result == WAIT_TIMEOUT ? "WAIT_TIMEOUT" : "WAIT_FAILED");
        }

        // Verify fence advanced
        rz_gfx_syncpoint new_completed = ID3D12Fence_GetCompletedValue(frameSyncobj->dx12.fence);
        if (new_completed < frameSyncobj->waitSyncpoint) {
            RAZIX_RHI_LOG_ERROR("[WAIT ERR] fence did not advance: completed=%llu, expected>=%llu", new_completed, frameSyncobj->waitSyncpoint);
        }
    }
}

static void dx12_SubmitCmdBuf(rz_gfx_submit_desc submitDesc)
{
    RAZIX_RHI_ASSERT(submitDesc.cmdCount > 0, "Command buffer count must be greater than zero");
    RAZIX_RHI_ASSERT(submitDesc.pCmdBufs != NULL, "Command buffer array cannot be NULL");
    RAZIX_RHI_ASSERT(submitDesc.pFrameSyncobj != NULL, "Frame sync object cannot be NULL");
    RAZIX_RHI_ASSERT(submitDesc.pWaitSyncobjs == NULL || submitDesc.waitSyncobjCount > 0, "Wait sync object count must be greater than zero if wait sync objects are provided");
    RAZIX_RHI_ASSERT(submitDesc.pSignalSyncobjs == NULL || submitDesc.signalSyncobjCount > 0, "Signal sync object count must be greater than zero if signal sync objects are provided");

    // stack allocate the command list array, quick and dirty
    ID3D12GraphicsCommandList** cmdLists = alloca(sizeof(ID3D12GraphicsCommandList*) * submitDesc.cmdCount);
    for (uint32_t i = 0; i < submitDesc.cmdCount; i++)
        cmdLists[i] = submitDesc.pCmdBufs[i].dx12.cmdList;

    ID3D12CommandQueue_ExecuteCommandLists(DX12Context.directQ, submitDesc.cmdCount, (ID3D12CommandList**) cmdLists);

    // TODO: use signal and wait semaphores for better sync on queue submit in DX12
}

static void dx12_Present(rz_gfx_present_desc presentDesc)
{
    RAZIX_RHI_ASSERT(presentDesc.pSwapchain != NULL, "Swapchain cannot be NULL");
    RAZIX_RHI_ASSERT(presentDesc.pFrameSyncobj != NULL, "Frame sync object cannot be NULL");
    RAZIX_RHI_ASSERT(presentDesc.pWaitSyncobjs == NULL || presentDesc.waitSyncobjCount > 0, "Wait sync object count must be greater than zero if wait sync objects are provided");

    // TODO: Allow tearing only when vsync is off in GfxCtxInitDesc
    CHECK_HR(IDXGISwapChain4_Present(presentDesc.pSwapchain->dx12.swapchain4, 0, DXGI_PRESENT_ALLOW_TEARING));

    // TODO: use wait semaphores for better sync on present in DX12

    // Signal the frame sync object to increment the fence value
    rz_gfx_syncpoint signalSyncpoint = ++presentDesc.pFrameSyncobj->waitSyncpoint;
    CHECK_HR(ID3D12CommandQueue_Signal(DX12Context.directQ, presentDesc.pFrameSyncobj->dx12.fence, signalSyncpoint));
}

static void dx12_BeginCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    CHECK_HR(ID3D12CommandAllocator_Reset(cmdBuf->dx12.cmdAlloc));
    CHECK_HR(ID3D12GraphicsCommandList_Reset(cmdBuf->dx12.cmdList, cmdBuf->dx12.cmdAlloc, NULL));
}

static void dx12_EndCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    CHECK_HR(ID3D12GraphicsCommandList_Close(cmdBuf->dx12.cmdList));
}

static void dx12_BeginRenderPass(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_renderpass* renderPass)
{
    ID3D12GraphicsCommandList* cmdList = cmdBuf->dx12.cmdList;

    // Set Scissor and Rects
    D3D12_VIEWPORT vp = {
        .TopLeftX = 0,
        .TopLeftY = 0,
        .Width    = (FLOAT) RAZIX_X(renderPass->extents),
        .Height   = (FLOAT) RAZIX_Y(renderPass->extents),
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f};
    ID3D12GraphicsCommandList_RSSetViewports(cmdBuf->dx12.cmdList, 1, &vp);

    D3D12_RECT scissor = {
        .left   = 0,
        .top    = 0,
        .right  = (LONG) RAZIX_X(renderPass->extents),
        .bottom = (LONG) RAZIX_Y(renderPass->extents)};
    ID3D12GraphicsCommandList_RSSetScissorRects(cmdBuf->dx12.cmdList, 1, &scissor);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[RAZIX_MAX_RENDER_TARGETS] = {0};
    uint32_t                    rtvCount                             = renderPass->colorAttachmentsCount;

    for (uint32_t i = 0; i < rtvCount; ++i) {
        rtvHandles[i] = renderPass->colorAttachments[i].pResourceView->dx12.rtv.cpu;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = {0};
    bool                        hasDepth  = renderPass->depthAttachment.pResourceView != NULL;

    if (hasDepth) {
        dsvHandle = renderPass->depthAttachment.pResourceView->dx12.dsv.cpu;
    }

    for (uint32_t i = 0; i < rtvCount; ++i) {
        if (renderPass->colorAttachments[i].clear) {
            float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
            memcpy(clearColor, renderPass->colorAttachments[i].clearColor.raw, sizeof(float) * 4);
            ID3D12GraphicsCommandList_ClearRenderTargetView(cmdList, rtvHandles[i], clearColor, 0, NULL);
        }
    }

    if (hasDepth && renderPass->depthAttachment.clear) {
        ID3D12GraphicsCommandList_ClearDepthStencilView(cmdList, dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);
    }

    ID3D12GraphicsCommandList_OMSetRenderTargets(cmdList, rtvCount, rtvHandles, FALSE, hasDepth ? &dsvHandle : NULL);
}

static void dx12_EndRenderPass(const rz_gfx_cmdbuf* cmdBuf)
{
    // In DX12 there's no explicit EndRenderPass unless using newer RenderPass APIs.
    // For basic OMSetRenderTargets path, this is a no-op.
    (void) cmdBuf;
}

static void dx12_SetViewport(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_viewport* viewport)
{
    D3D12_VIEWPORT vp = {
        .TopLeftX = (FLOAT) viewport->x,
        .TopLeftY = (FLOAT) viewport->y,
        .Width    = (FLOAT) viewport->width,
        .Height   = (FLOAT) viewport->height,
        .MinDepth = (FLOAT) viewport->minDepth,
        .MaxDepth = (FLOAT) viewport->maxDepth};
    ID3D12GraphicsCommandList_RSSetViewports(cmdBuf->dx12.cmdList, 1, &vp);
}

static void dx12_SetScissorRect(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_rect* rect)
{
    D3D12_RECT scissor = {
        .left   = (LONG) rect->x,
        .top    = (LONG) rect->y,
        .right  = (LONG) rect->x + rect->width,
        .bottom = (LONG) rect->y + rect->height};
    ID3D12GraphicsCommandList_RSSetScissorRects(cmdBuf->dx12.cmdList, 1, &scissor);
}

static void dx12_BindPipeline(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_pipeline* pso)
{
    if (pso->resource.desc.pipelineDesc.type == RZ_GFX_PIPELINE_TYPE_GRAPHICS)
        ID3D12GraphicsCommandList_IASetPrimitiveTopology(cmdBuf->dx12.cmdList, pso->dx12.topology);
    ID3D12GraphicsCommandList_SetPipelineState(cmdBuf->dx12.cmdList, pso->dx12.pso);
}

static void dx12_BindGfxRootSig(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_root_signature* rootSig)
{
    ID3D12GraphicsCommandList_SetGraphicsRootSignature(cmdBuf->dx12.cmdList, rootSig->dx12.rootSig);
}

static void dx12_BindComputeRootSig(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_root_signature* rootSig)
{
    ID3D12GraphicsCommandList_SetComputeRootSignature(cmdBuf->dx12.cmdList, rootSig->dx12.rootSig);
}

static void dx12_BindDescriptorHeaps(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_descriptor_heap** heaps, uint32_t heapCount)
{
    RAZIX_RHI_ASSERT(heaps != NULL, "Heaps cannot be NULL");
    RAZIX_RHI_ASSERT(heapCount > 0 && heapCount <= RAZIX_MAX_ALLOWED_HEAPS_TO_BIND, "Invalid heap count: %d. Must be between 1 and %d", heapCount, RAZIX_MAX_ALLOWED_HEAPS_TO_BIND);

    ID3D12DescriptorHeap* dx12Heaps[RAZIX_MAX_ALLOWED_HEAPS_TO_BIND] = {0};
    for (uint32_t i = 0; i < heapCount; ++i) {
        dx12Heaps[i] = heaps[i]->dx12.heap;
    }
    ID3D12GraphicsCommandList_SetDescriptorHeaps(cmdBuf->dx12.cmdList, heapCount, dx12Heaps);
}

static void dx12_BindDescriptorTables(const rz_gfx_cmdbuf* cmdBuf, rz_gfx_pipeline_type pipelineType, const rz_gfx_root_signature* rootSig, const rz_gfx_descriptor_table** tables, uint32_t tableCount)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be null");
    RAZIX_RHI_ASSERT(tables != NULL, "Tables cannot be NULL");
    RAZIX_RHI_ASSERT(tableCount > 0 && tableCount <= RAZIX_MAX_ALLOWED_TABLES_TO_BIND, "Invalid table count: %d. Must be between 1 and %d", tableCount, RAZIX_MAX_ALLOWED_TABLES_TO_BIND);

    // Vulkan uses the root signature to bind descriptor tables, it needs VkPipelineLayout to bind descriptor sets
    // Unused in DX12 as root signature is set directly on cmd list
    (void) rootSig;

    ID3D12DescriptorHeap* dx12Heaps[RAZIX_MAX_ALLOWED_TABLES_TO_BIND] = {0};
    for (uint32_t i = 0; i < tableCount; i++) {
        dx12Heaps[i] = tables[i]->resource.desc.descriptorTableDesc.pHeap->dx12.heap;
        if (pipelineType == RZ_GFX_PIPELINE_TYPE_GRAPHICS)
            ID3D12GraphicsCommandList_SetGraphicsRootDescriptorTable(cmdBuf->dx12.cmdList, i /*tables[i]->resource.desc.descriptorTableDesc.tableIndex*/, tables[i]->dx12.heapOffset.gpu);
        else if (pipelineType == RZ_GFX_PIPELINE_TYPE_COMPUTE)
            ID3D12GraphicsCommandList_SetComputeRootDescriptorTable(cmdBuf->dx12.cmdList, i /*tables[i]->resource.desc.descriptorTableDesc.tableIndex*/, tables[i]->dx12.heapOffset.gpu);
        else
            RAZIX_RHI_LOG_ERROR("Unsupported pipeline type for binding descriptor tables: %d", pipelineType);
    }
}

static void dx12_BindVertexBuffers(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* const* buffers, uint32_t bufferCount, const uint32_t* offsets, const uint32_t* strides)
{
    RAZIX_RHI_ASSERT(buffers != NULL, "Buffers cannot be NULL");
    RAZIX_RHI_ASSERT(offsets != NULL, "Offsets cannot be NULL");
    RAZIX_RHI_ASSERT(strides != NULL, "Strides cannot be NULL");
    RAZIX_RHI_ASSERT(bufferCount > 0 && bufferCount <= RAZIX_MAX_VERTEX_BUFFERS_BOUND, "Invalid buffer count: %d. Must be between 1 and %d", bufferCount, RAZIX_MAX_VERTEX_BUFFERS_BOUND);
    D3D12_VERTEX_BUFFER_VIEW vbViews[RAZIX_MAX_VERTEX_BUFFERS_BOUND] = {0};
    for (uint32_t i = 0; i < bufferCount; ++i) {
        RAZIX_RHI_ASSERT(buffers[i] != NULL, "Buffer at index %d is NULL", i);
        ID3D12Resource* d3d_buffer = (ID3D12Resource*) buffers[i]->dx12.resource;
        vbViews[i].BufferLocation  = ID3D12Resource_GetGPUVirtualAddress(d3d_buffer) + offsets[i];
        vbViews[i].SizeInBytes     = (UINT) buffers[i]->resource.desc.bufferDesc.sizeInBytes - offsets[i];
        vbViews[i].StrideInBytes   = strides[i];
    }
    ID3D12GraphicsCommandList_IASetVertexBuffers(cmdBuf->dx12.cmdList, 0, bufferCount, vbViews);
}

static void dx12_BindIndexBuffer(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* buffer, uint32_t offset, rz_gfx_index_type indexType)
{
    RAZIX_RHI_ASSERT(buffer != NULL, "Buffer cannot be NULL");
    ID3D12Resource*         d3d_buffer = (ID3D12Resource*) buffer->dx12.resource;
    D3D12_INDEX_BUFFER_VIEW ibView;
    ibView.BufferLocation = ID3D12Resource_GetGPUVirtualAddress(d3d_buffer) + offset;
    ibView.SizeInBytes    = (UINT) buffer->resource.desc.bufferDesc.sizeInBytes - offset;
    if (indexType == RZ_GFX_INDEX_TYPE_UINT16)
        ibView.Format = DXGI_FORMAT_R16_UINT;
    else if (indexType == RZ_GFX_INDEX_TYPE_UINT32)
        ibView.Format = DXGI_FORMAT_R32_UINT;
    else {
        RAZIX_RHI_LOG_ERROR("Unsupported index type: %d", indexType);
        return;
    }
    ID3D12GraphicsCommandList_IASetIndexBuffer(cmdBuf->dx12.cmdList, &ibView);
}

static void dx12_DrawAuto(const rz_gfx_cmdbuf* cmdBuf, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(vertexCount > 0, "Vertex count must be greater than zero");
    RAZIX_RHI_ASSERT(instanceCount > 0, "Instance count must be greater than zero");

    ID3D12GraphicsCommandList_DrawInstanced(cmdBuf->dx12.cmdList, vertexCount, instanceCount, firstVertex, firstInstance);
}

static void dx12_DrawIndexedAuto(const rz_gfx_cmdbuf* cmdBuf, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(indexCount > 0, "Index count must be greater than zero");
    RAZIX_RHI_ASSERT(instanceCount > 0, "Instance count must be greater than zero");

    ID3D12GraphicsCommandList_DrawIndexedInstanced(cmdBuf->dx12.cmdList, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

static void dx12_Dispatch(const rz_gfx_cmdbuf* cmdBuf, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(groupCountX > 0, "Group count X must be greater than zero");
    RAZIX_RHI_ASSERT(groupCountY > 0, "Group count Y must be greater than zero");
    RAZIX_RHI_ASSERT(groupCountZ > 0, "Group count Z must be greater than zero");

    ID3D12GraphicsCommandList_Dispatch(cmdBuf->dx12.cmdList, groupCountX, groupCountY, groupCountZ);
}

static void dx12_DrawIndirect(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* indirectBuffer, uint32_t offset, uint32_t drawCount)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(indirectBuffer != NULL, "Indirect buffer cannot be NULL");
    RAZIX_RHI_ASSERT(drawCount > 0, "Draw count must be greater than zero");

    ID3D12Resource* d3d_buffer = (ID3D12Resource*) indirectBuffer->dx12.resource;

    ID3D12GraphicsCommandList_ExecuteIndirect(
        cmdBuf->dx12.cmdList,
        DX12Context.drawIndirectSignature,
        drawCount,
        d3d_buffer,
        offset,
        NULL,    // Count buffer (optional)
        0        // Count buffer offset
    );
}

static void dx12_DrawIndexedIndirect(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* indirectBuffer, uint32_t offset, uint32_t drawCount)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(indirectBuffer != NULL, "Indirect buffer cannot be NULL");
    RAZIX_RHI_ASSERT(drawCount > 0, "Draw count must be greater than zero");

    ID3D12Resource* d3d_buffer = (ID3D12Resource*) indirectBuffer->dx12.resource;

    ID3D12GraphicsCommandList_ExecuteIndirect(
        cmdBuf->dx12.cmdList,
        DX12Context.drawIndexedIndirectSignature,
        drawCount,
        d3d_buffer,
        offset,
        NULL,    // Count buffer (optional)
        0        // Count buffer offset
    );
}

static void dx12_DispatchIndirect(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* indirectBuffer, uint32_t offset, uint32_t dispatchCount)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(indirectBuffer != NULL, "Indirect buffer cannot be NULL");
    RAZIX_RHI_ASSERT(dispatchCount > 0, "Dispatch count must be greater than zero");

    ID3D12Resource* d3d_buffer = (ID3D12Resource*) indirectBuffer->dx12.resource;

    ID3D12GraphicsCommandList_ExecuteIndirect(
        cmdBuf->dx12.cmdList,
        DX12Context.dispatchIndirectSignature,
        dispatchCount,    // maxCommandCount - always 1 for dispatch
        d3d_buffer,
        offset,
        NULL,    // Count buffer (optional)
        0        // Count buffer offset
    );
}

static void dx12_UpdateDescriptorTable(rz_gfx_descriptor_table_update tableUpdate)
{
    rz_gfx_descriptor_table* pTable = (rz_gfx_descriptor_table*) tableUpdate.pTable;
    RAZIX_RHI_ASSERT(pTable != NULL, "Descriptor table cannot be NULL");
    RAZIX_RHI_ASSERT(tableUpdate.pResourceViews != NULL, "Resource views cannot be NULL for updating descriptor table");
    RAZIX_RHI_ASSERT(tableUpdate.resViewCount > 0, "Resource view count must be greater than zero for updating descriptor table");

    for (uint32_t i = 0; i < tableUpdate.resViewCount; i++) {
        const rz_gfx_resource_view*      pView     = &tableUpdate.pResourceViews[i];
        const rz_gfx_resource_view_desc* pViewDesc = &pView->resource.desc.resourceViewDesc;
        RAZIX_RHI_ASSERT(pView != NULL, "Resource view cannot be NULL in a descriptor table! (Descriptor Table creation)");
        RAZIX_RHI_ASSERT(pViewDesc != NULL, "Resource view descriptor cannot be NULL in a descriptor table! (Descriptor Table creation)");

        bool isTexture        = rzRHI_IsDescriptorTypeTexture(pViewDesc->descriptorType);
        bool isBuffer         = rzRHI_IsDescriptorTypeBuffer(pViewDesc->descriptorType);
        bool isSampler        = pViewDesc->descriptorType == RZ_GFX_DESCRIPTOR_TYPE_SAMPLER;
        bool isConstantBuffer = pViewDesc->descriptorType == RZ_GFX_DESCRIPTOR_TYPE_CONSTANT_BUFFER;
        RAZIX_RHI_ASSERT(isTexture || isBuffer || isSampler || isConstantBuffer, "Descriptor type must be a texture, buffer, constant buffer or sampler for descriptor table creation");
        bool isTextureRW = isTexture && rzRHI_IsDescriptorTypeTextureRW(pViewDesc->descriptorType);
        bool isBufferRW  = isBuffer && rzRHI_IsDescriptorTypeBufferRW(pViewDesc->descriptorType);

        if (isConstantBuffer) {
            ID3D12Device_CreateConstantBufferView(DX12Device, &pView->dx12.cbvDesc, pTable->dx12.heapOffset.cpu);
        } else if (isTexture && !isTextureRW) {
            const rz_gfx_texture* pTexture = pViewDesc->textureViewDesc.pTexture;
            RAZIX_RHI_ASSERT(pTexture != NULL, "Texture cannot be NULL for descriptor table SRV creation");
            ID3D12Device_CreateShaderResourceView(DX12Device, pTexture->dx12.resource, &pView->dx12.srvDesc, pTable->dx12.heapOffset.cpu);
        } else if (isTextureRW) {
            const rz_gfx_texture* pTexture = pViewDesc->textureViewDesc.pTexture;
            RAZIX_RHI_ASSERT(pTexture != NULL, "Texture cannot be NULL for descriptor table UAV creation");
            ID3D12Device_CreateUnorderedAccessView(DX12Device, pTexture->dx12.resource, NULL, &pView->dx12.uavDesc, pTable->dx12.heapOffset.cpu);
        } else if (isBuffer && !isBufferRW) {
            const rz_gfx_buffer* pBuffer = pViewDesc->bufferViewDesc.pBuffer;
            RAZIX_RHI_ASSERT(pBuffer != NULL, "Buffer cannot be NULL for descriptor table SRV creation");
            ID3D12Device_CreateShaderResourceView(DX12Device, pBuffer->dx12.resource, &pView->dx12.srvDesc, pTable->dx12.heapOffset.cpu);
        } else if (isBufferRW) {
            const rz_gfx_buffer* pBuffer = pViewDesc->bufferViewDesc.pBuffer;
            RAZIX_RHI_ASSERT(pBuffer != NULL, "Buffer cannot be NULL for descriptor table UAV creation");
            ID3D12Device_CreateUnorderedAccessView(DX12Device, pBuffer->dx12.resource, NULL, &pView->dx12.uavDesc, pTable->dx12.heapOffset.cpu);
        } else if (isSampler) {
            ID3D12Device_CreateSampler(DX12Device, &pView->dx12.samplerDesc, pTable->dx12.heapOffset.cpu);
        } else if (pViewDesc->descriptorType == RZ_GFX_DESCRIPTOR_TYPE_PUSH_CONSTANT) {
            RAZIX_RHI_LOG_ERROR("Seriously? RZ_GFX_DESCRIPTOR_TYPE_PUSH_CONSTANT in here? bind it directly on the correct root signature. This will result in catastrophic descriptor API failure.");
            RAZIX_RHI_ABORT();
            return;
        } else if (pViewDesc->descriptorType == RZ_GFX_DESCRIPTOR_TYPE_IMAGE_SAMPLER_COMBINED) {
            RAZIX_RHI_LOG_ERROR("RZ_GFX_DESCRIPTOR_TYPE_IMAGE_SAMPLER_COMBINED is Vulkan only and not recommended with DX12 backend. This will result in catastrophic descriptor API failure.");
            RAZIX_RHI_ABORT();
            return;
        } else if (pViewDesc->descriptorType == RZ_GFX_DESCRIPTOR_TYPE_RENDER_TEXTURE || pViewDesc->descriptorType == RZ_GFX_DESCRIPTOR_TYPE_DEPTH_STENCIL_TEXTURE) {
            RAZIX_RHI_LOG_ERROR("Seriously? RZ_GFX_DESCRIPTOR_TYPE_RENDER_TEXTURE/DEPTH_STENCIL in here? create the resource views and pass them to BeginRenderPass directly to set using OMSetXXX. This will result in catastrophic descriptor API failure.");
            RAZIX_RHI_ABORT();
            return;
        } else {
            RAZIX_RHI_LOG_ERROR("Unsupported descriptor type: %d. (This is likely a bug in the if-else chain or weird data corruption has happened). This will result in catastrophic descriptor API failure.", pViewDesc->descriptorType);
            RAZIX_RHI_ABORT();
            return;
        }
    }
}

static void dx12_UpdateConstantBuffer(rz_gfx_buffer_update updatedesc)
{
    RAZIX_RHI_ASSERT(updatedesc.pBuffer != NULL, "Buffer cannot be NULL");
    RAZIX_RHI_ASSERT(updatedesc.sizeInBytes > 0, "Size in bytes must be greater than zero");
    RAZIX_RHI_ASSERT(updatedesc.offset + updatedesc.sizeInBytes <= updatedesc.pBuffer->resource.desc.bufferDesc.sizeInBytes, "Update range exceeds buffer size");
    RAZIX_RHI_ASSERT(updatedesc.pData != NULL, "Data pointer cannot be NULL");
    RAZIX_RHI_ASSERT((updatedesc.pBuffer->resource.desc.bufferDesc.usage & RZ_GFX_BUFFER_USAGE_TYPE_DYNAMIC) == RZ_GFX_BUFFER_USAGE_TYPE_DYNAMIC, "Buffer must be created with RZ_GFX_BUFFER_USAGE_TYPE_DYNAMIC usage flag");
    RAZIX_RHI_ASSERT((updatedesc.pBuffer->resource.desc.bufferDesc.type & RZ_GFX_BUFFER_TYPE_CONSTANT) == RZ_GFX_BUFFER_TYPE_CONSTANT, "Buffer must be of type RZ_GFX_BUFFER_TYPE_CONSTANT to update");

    D3D12_RANGE readRange = {0};
    readRange.Begin       = updatedesc.offset;
    readRange.End         = updatedesc.offset + updatedesc.sizeInBytes;
    void* mappedData      = NULL;
    ID3D12Resource_Map(updatedesc.pBuffer->dx12.resource, 0, &readRange, &mappedData);
    RAZIX_RHI_ASSERT(mappedData != NULL, "Failed to map constant buffer memory");
    memcpy((uint8_t*) mappedData, updatedesc.pData, updatedesc.sizeInBytes);
}

static void dx12_InsertImageBarrier(const rz_gfx_cmdbuf* cmdBuf, rz_gfx_texture* texture, rz_gfx_resource_state beforeState, rz_gfx_resource_state afterState)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(texture != NULL, "Texture cannot be NULL");
    RAZIX_RHI_ASSERT(beforeState != RZ_GFX_RESOURCE_STATE_UNDEFINED, "Before state cannot be undefined");
    RAZIX_RHI_ASSERT(afterState != RZ_GFX_RESOURCE_STATE_UNDEFINED, "After state cannot be undefined");
    RAZIX_RHI_ASSERT(!(texture->resource.viewHints & RZ_GFX_RESOURCE_VIEW_FLAG_UAV) ||
                         (beforeState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS || afterState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS),
        "UAV barriers must be used only with UAV resources. If the resource has UAV view hint, either before or after state must be UAV");
    RAZIX_RHI_ASSERT(!(beforeState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS && afterState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS) ||
                         (texture->resource.viewHints & RZ_GFX_RESOURCE_VIEW_FLAG_UAV),
        "UAV-to-UAV barrier requires resource to have UAV view hint");

    bool isUAVBarrier = (beforeState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS && afterState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS);

    if (isUAVBarrier) {
        D3D12_RESOURCE_BARRIER uavBarrier = {
            .Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV,
            .UAV.pResource = texture->dx12.resource};
        ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 1, &uavBarrier);
    }

    if (beforeState == afterState)
        return;

    D3D12_RESOURCE_BARRIER barrier = {
        .Type       = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition = {
            .pResource   = texture->dx12.resource,
            .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,    // TODO: use mip/layer combo
            .StateBefore = dx12_util_res_state_translate(beforeState),
            .StateAfter  = dx12_util_res_state_translate(afterState),
        }};

    // Update the current state
    texture->resource.currentState = afterState;

    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 1, &barrier);
}

static void dx12_InsertBufferBarrier(const rz_gfx_cmdbuf* cmdBuf, rz_gfx_buffer* buffer, rz_gfx_resource_state beforeState, rz_gfx_resource_state afterState)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(buffer != NULL, "Buffer cannot be NULL");
    RAZIX_RHI_ASSERT(beforeState != RZ_GFX_RESOURCE_STATE_UNDEFINED, "Before state cannot be undefined");
    RAZIX_RHI_ASSERT(afterState != RZ_GFX_RESOURCE_STATE_UNDEFINED, "After state cannot be undefined");
    RAZIX_RHI_ASSERT(!(buffer->resource.viewHints & RZ_GFX_RESOURCE_VIEW_FLAG_UAV) ||
                         (beforeState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS || afterState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS),
        "UAV barriers must be used only with UAV resources. If the resource has UAV view hint, either before or after state must be UAV");
    RAZIX_RHI_ASSERT(!(beforeState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS && afterState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS) ||
                         (buffer->resource.viewHints & RZ_GFX_RESOURCE_VIEW_FLAG_UAV),
        "UAV-to-UAV barrier requires resource to have UAV view hint");

    bool isUAVBarrier = (beforeState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS && afterState == RZ_GFX_RESOURCE_STATE_UNORDERED_ACCESS);

    if (isUAVBarrier) {
        D3D12_RESOURCE_BARRIER uavBarrier = {
            .Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV,
            .UAV.pResource = buffer->dx12.resource};
        ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 1, &uavBarrier);
    }

    if (beforeState == afterState)
        return;

    D3D12_RESOURCE_BARRIER barrier = {
        .Type       = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition = {
            .pResource   = buffer->dx12.resource,
            .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            .StateBefore = dx12_util_res_state_translate(beforeState),
            .StateAfter  = dx12_util_res_state_translate(afterState),
        }};

    // Update the current state
    buffer->resource.currentState = afterState;
    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 1, &barrier);
}

static void dx12_InsertTextureReadback(const rz_gfx_texture* texture, rz_gfx_texture_readback* readback)
{
    RAZIX_RHI_ASSERT(texture != NULL, "Texture cannot be NULL");
    RAZIX_RHI_ASSERT(readback != NULL, "Readback structure cannot be NULL");

    ID3D12Resource*     srcResource = texture->dx12.resource;
    D3D12_RESOURCE_DESC srcDesc     = {0};
    ID3D12Resource_GetDesc(srcResource, &srcDesc);

    dx12_cmdbuf cmdBuf = dx12_util_begin_singletime_cmdlist();

    uint32_t width  = texture->resource.desc.textureDesc.width;
    uint32_t height = texture->resource.desc.textureDesc.height;
    uint32_t size   = width * height * 4;    // Assuming 4 bytes per pixel (RGBA8)

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT srcFootprint = {0};
    UINT64                             totalSize    = 0;
    UINT64                             rowPitch     = 0;
    UINT                               numRows      = 0;
    ID3D12Device_GetCopyableFootprints(DX12Device, &srcDesc, 0, 1, 0, &srcFootprint, &numRows, &rowPitch, &totalSize);

    RAZIX_RHI_ASSERT(totalSize > 0, "[DX12] Invalid total size returned for texture readback!");
    RAZIX_RHI_ASSERT(rowPitch > 0, "[DX12] Invalid row pitch returned!");
    RAZIX_RHI_ASSERT(numRows > 0, "[DX12] Invalid number of rows!");
    RAZIX_RHI_ASSERT(srcFootprint.Footprint.RowPitch > 0, "[DX12] Footprint row pitch is invalid!");
    RAZIX_RHI_ASSERT(srcFootprint.Footprint.Width > 0 && srcFootprint.Footprint.Height > 0, "[DX12] Invalid footprint dimensions!");

    ID3D12Resource* readbackBuffer = NULL;

    D3D12_HEAP_PROPERTIES heap_props = {
        .Type                 = D3D12_HEAP_TYPE_READBACK,
        .CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
        .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN};

    D3D12_RESOURCE_DESC buffer_desc = {
        .Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment        = 0,
        .Width            = totalSize,
        .Height           = 1,
        .DepthOrArraySize = 1,
        .MipLevels        = 1,
        .Format           = DXGI_FORMAT_UNKNOWN,
        .SampleDesc       = {1, 0},
        .Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags            = D3D12_RESOURCE_FLAG_NONE};

    CHECK_HR(ID3D12Device_CreateCommittedResource(
        DX12Device,
        &heap_props,
        D3D12_HEAP_FLAG_NONE,
        &buffer_desc,
        D3D12_RESOURCE_STATE_COPY_DEST,    // Directly created with copy dest state
        NULL,
        &IID_ID3D12Resource,
        (void**) &readbackBuffer));

    D3D12_RESOURCE_BARRIER barrier = {
        .Type       = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition = {
            .pResource   = srcResource,
            .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            .StateBefore = dx12_util_res_state_translate(texture->resource.currentState),
            .StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE}};
    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf.cmdList, 1, &barrier);

    D3D12_TEXTURE_COPY_LOCATION src = {
        .pResource        = srcResource,
        .Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        .SubresourceIndex = 0};

    D3D12_TEXTURE_COPY_LOCATION dst = {
        .pResource       = readbackBuffer,
        .Type            = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
        .PlacedFootprint = srcFootprint};

    ID3D12GraphicsCommandList_CopyTextureRegion(cmdBuf.cmdList, &dst, 0, 0, 0, &src, NULL);

    D3D12_RESOURCE_BARRIER restore_barrier = {
        .Type       = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition = {
            .pResource   = srcResource,
            .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            .StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE,
            .StateAfter  = dx12_util_res_state_translate(texture->resource.currentState)}};
    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf.cmdList, 1, &restore_barrier);

    // Close command list and submit and flush GPU work
    dx12_util_end_singletime_cmdlist(cmdBuf);

    void*       mapped_data = NULL;
    D3D12_RANGE read_range  = {0, totalSize};
    CHECK_HR(ID3D12Resource_Map(readbackBuffer, 0, &read_range, &mapped_data));

    readback->width  = width;
    readback->height = height;
    readback->bpp    = 32;
    // TODO: Get a malloc CB from user, and use that instead of allocating memory from RHI, RHI should not be allocating memory for user in any shape or form
    // Note: Pray to god that user will free this memory
    readback->data = malloc(size);

    if (readback->data) {
        for (UINT y = 0; y < height; ++y) {
            uint8_t* row_src = (uint8_t*) mapped_data + srcFootprint.Footprint.RowPitch * y;
            uint8_t* row_dst = (uint8_t*) readback->data + width * 4 * y;
            memcpy(row_dst, row_src, width * 4);
        }
    }

    D3D12_RANGE write_range = {0, 0};
    ID3D12Resource_Unmap(readbackBuffer, 0, &write_range);
    ID3D12Resource_Release(readbackBuffer);
}

static void dx12_InsertBufferReadback(const rz_gfx_buffer* buffer, rz_gfx_buffer_readback* readback)
{
    RAZIX_RHI_ASSERT(buffer != NULL, "Buffer cannot be NULL");
    RAZIX_RHI_ASSERT(readback != NULL, "Readback structure cannot be NULL");

    ID3D12Resource*     srcResource = buffer->dx12.resource;
    D3D12_RESOURCE_DESC srcDesc     = {0};
    ID3D12Resource_GetDesc(srcResource, &srcDesc);

    dx12_cmdbuf cmdBuf = dx12_util_begin_singletime_cmdlist();

    uint32_t              size           = (uint32_t) srcDesc.Width;
    ID3D12Resource*       readbackBuffer = NULL;
    D3D12_HEAP_PROPERTIES heap_props     = {
            .Type                 = D3D12_HEAP_TYPE_READBACK,
            .CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
            .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN};
    D3D12_RESOURCE_DESC buffer_desc = {
        .Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment        = 0,
        .Width            = size,
        .Height           = 1,
        .DepthOrArraySize = 1,
        .MipLevels        = 1,
        .Format           = DXGI_FORMAT_UNKNOWN,
        .SampleDesc       = {1, 0},
        .Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags            = D3D12_RESOURCE_FLAG_NONE};
    CHECK_HR(ID3D12Device_CreateCommittedResource(
        DX12Device,
        &heap_props,
        D3D12_HEAP_FLAG_NONE,
        &buffer_desc,
        D3D12_RESOURCE_STATE_COPY_DEST,    // Directly created with copy dest state
        NULL,
        &IID_ID3D12Resource,
        (void**) &readbackBuffer));
    D3D12_RESOURCE_BARRIER barrier = {
        .Type       = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition = {
            .pResource   = srcResource,
            .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            .StateBefore = dx12_util_res_state_translate(buffer->resource.currentState),
            .StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE}};
    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf.cmdList, 1, &barrier);

    ID3D12GraphicsCommandList_CopyBufferRegion(cmdBuf.cmdList, readbackBuffer, 0, srcResource, 0, size);

    D3D12_RESOURCE_BARRIER restore_barrier = {
        .Type       = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition = {
            .pResource   = srcResource,
            .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
            .StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE,
            .StateAfter  = dx12_util_res_state_translate(buffer->resource.currentState)}};
    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf.cmdList, 1, &restore_barrier);

    // Close command list and submit and flush GPU work
    dx12_util_end_singletime_cmdlist(cmdBuf);
    void*       mapped_data = NULL;
    D3D12_RANGE read_range  = {readback->offset, size + readback->offset};
    CHECK_HR(ID3D12Resource_Map(readbackBuffer, 0, &read_range, &mapped_data));
    readback->sizeInBytes = size;
}

static void dx12_CopyBuffer(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* src, const rz_gfx_buffer* dst, uint32_t size, uint32_t srcOffset, uint32_t dstOffset)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(src != NULL, "Source buffer cannot be NULL");
    RAZIX_RHI_ASSERT(dst != NULL, "Destination buffer cannot be NULL");
    RAZIX_RHI_ASSERT(size > 0, "Size must be greater than zero");
    RAZIX_RHI_ASSERT(srcOffset + size <= src->resource.desc.bufferDesc.sizeInBytes, "Source buffer copy range exceeds buffer size");
    RAZIX_RHI_ASSERT(dstOffset + size <= dst->resource.desc.bufferDesc.sizeInBytes, "Destination buffer copy range exceeds buffer size");

    D3D12_RESOURCE_STATES originalSrcState = dx12_util_res_state_translate(src->resource.currentState);
    D3D12_RESOURCE_STATES originalDstState = dx12_util_res_state_translate(dst->resource.currentState);

    D3D12_RESOURCE_BARRIER transitionToCopyBarriers[2] = {
        {.Type          = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition = {
                .pResource   = src->dx12.resource,
                .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .StateBefore = originalSrcState,
                .StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE}},
        {.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .Transition = {.pResource = dst->dx12.resource, .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, .StateBefore = originalDstState, .StateAfter = D3D12_RESOURCE_STATE_COPY_DEST}}};

    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 2, transitionToCopyBarriers);

    ID3D12GraphicsCommandList_CopyBufferRegion(cmdBuf->dx12.cmdList, dst->dx12.resource, dstOffset, src->dx12.resource, srcOffset, size);

    D3D12_RESOURCE_BARRIER restoreBarriers[2] = {
        {.Type          = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition = {
                .pResource   = src->dx12.resource,
                .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE,
                .StateAfter  = originalSrcState}},
        {.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .Transition = {.pResource = dst->dx12.resource, .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, .StateBefore = D3D12_RESOURCE_STATE_COPY_DEST, .StateAfter = originalDstState}}};

    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 2, restoreBarriers);
}

static void dx12_CopyTexture(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_texture* src, const rz_gfx_texture* dst)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(src != NULL, "Source texture cannot be NULL");
    RAZIX_RHI_ASSERT(dst != NULL, "Destination texture cannot be NULL");
    RAZIX_RHI_ASSERT(src->resource.desc.textureDesc.width == dst->resource.desc.textureDesc.width &&
                         src->resource.desc.textureDesc.height == dst->resource.desc.textureDesc.height &&
                         src->resource.desc.textureDesc.depth == dst->resource.desc.textureDesc.depth,
        "Source and destination textures must have the same dimensions for copy");

    D3D12_RESOURCE_STATES originalSrcState = dx12_util_res_state_translate(src->resource.currentState);
    D3D12_RESOURCE_STATES originalDstState = dx12_util_res_state_translate(dst->resource.currentState);

    D3D12_RESOURCE_BARRIER transitionToCopyBarriers[2] = {
        {.Type          = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition = {
                .pResource   = src->dx12.resource,
                .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .StateBefore = originalSrcState,
                .StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE}},
        {.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .Transition = {.pResource = dst->dx12.resource, .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, .StateBefore = originalDstState, .StateAfter = D3D12_RESOURCE_STATE_COPY_DEST}}};

    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 2, transitionToCopyBarriers);

    D3D12_TEXTURE_COPY_LOCATION srcLocation = {
        .pResource        = src->dx12.resource,
        .Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        .SubresourceIndex = 0};
    D3D12_TEXTURE_COPY_LOCATION dstLocation = {
        .pResource        = dst->dx12.resource,
        .Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        .SubresourceIndex = 0};
    ID3D12GraphicsCommandList_CopyTextureRegion(cmdBuf->dx12.cmdList, &dstLocation, 0, 0, 0, &srcLocation, NULL);

    D3D12_RESOURCE_BARRIER restoreBarriers[2] = {
        {.Type          = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition = {
                .pResource   = src->dx12.resource,
                .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE,
                .StateAfter  = originalSrcState}},
        {.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .Transition = {.pResource = dst->dx12.resource, .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, .StateBefore = D3D12_RESOURCE_STATE_COPY_DEST, .StateAfter = originalDstState}}};

    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 2, restoreBarriers);
}

static void dx12_CopyBufferToTexture(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_buffer* src, const rz_gfx_texture* dst)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(src != NULL, "Source buffer cannot be NULL");
    RAZIX_RHI_ASSERT(dst != NULL, "Destination texture cannot be NULL");
    RAZIX_RHI_ASSERT(src->resource.desc.bufferDesc.sizeInBytes <= (dst->resource.desc.textureDesc.width * dst->resource.desc.textureDesc.height * 4),
        "Source buffer size is insufficient for the texture copy");

    D3D12_RESOURCE_STATES originalSrcState = dx12_util_res_state_translate(src->resource.currentState);
    D3D12_RESOURCE_STATES originalDstState = dx12_util_res_state_translate(dst->resource.currentState);

    D3D12_RESOURCE_BARRIER transitionToCopyBarriers[2] = {
        {.Type          = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition = {
                .pResource   = src->dx12.resource,
                .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .StateBefore = originalSrcState,
                .StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE}},
        {.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .Transition = {.pResource = dst->dx12.resource, .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, .StateBefore = originalDstState, .StateAfter = D3D12_RESOURCE_STATE_COPY_DEST}}};

    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 2, transitionToCopyBarriers);

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT dstFootprint = {0};
    UINT64                             totalSize    = 0;
    UINT64                             rowPitch     = 0;
    UINT                               numRows      = 0;
    D3D12_RESOURCE_DESC                dstDesc      = {0};
    ID3D12Resource_GetDesc(dst->dx12.resource, &dstDesc);
    ID3D12Device_GetCopyableFootprints(DX12Device, &dstDesc, 0, 1, 0, &dstFootprint, &numRows, &rowPitch, &totalSize);

    D3D12_TEXTURE_COPY_LOCATION srcLocation = {
        .pResource       = src->dx12.resource,
        .Type            = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
        .PlacedFootprint = dstFootprint};
    D3D12_TEXTURE_COPY_LOCATION dstLocation = {
        .pResource        = dst->dx12.resource,
        .Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        .SubresourceIndex = 0};

    ID3D12GraphicsCommandList_CopyTextureRegion(cmdBuf->dx12.cmdList, &dstLocation, 0, 0, 0, &srcLocation, NULL);

    D3D12_RESOURCE_BARRIER restoreBarriers[2] = {
        {.Type          = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition = {
                .pResource   = src->dx12.resource,
                .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE,
                .StateAfter  = originalSrcState}},
        {.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .Transition = {.pResource = dst->dx12.resource, .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, .StateBefore = D3D12_RESOURCE_STATE_COPY_DEST, .StateAfter = originalDstState}}};

    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 2, restoreBarriers);
}

static void dx12_CopyTextureToBufferFn(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_texture* src, const rz_gfx_buffer* dst)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(src != NULL, "Source texture cannot be NULL");
    RAZIX_RHI_ASSERT(dst != NULL, "Destination buffer cannot be NULL");
    RAZIX_RHI_ASSERT(dst->resource.desc.bufferDesc.sizeInBytes >= (src->resource.desc.textureDesc.width * src->resource.desc.textureDesc.height * 4),
        "Destination buffer size is insufficient for the texture copy");

    D3D12_RESOURCE_STATES originalSrcState = dx12_util_res_state_translate(src->resource.currentState);
    D3D12_RESOURCE_STATES originalDstState = dx12_util_res_state_translate(dst->resource.currentState);

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT srcFootprint = {0};
    UINT64                             totalSize    = 0;
    UINT64                             rowPitch     = 0;
    UINT                               numRows      = 0;
    D3D12_RESOURCE_DESC                srcDesc      = {0};
    ID3D12Resource_GetDesc(src->dx12.resource, &srcDesc);
    ID3D12Device_GetCopyableFootprints(DX12Device, &srcDesc, 0, 1, 0, &srcFootprint, &numRows, &rowPitch, &totalSize);

    D3D12_RESOURCE_BARRIER transitionToCopyBarriers[2] = {
        {.Type          = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition = {
                .pResource   = src->dx12.resource,
                .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .StateBefore = originalSrcState,
                .StateAfter  = D3D12_RESOURCE_STATE_COPY_SOURCE}},
        {.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .Transition = {.pResource = dst->dx12.resource, .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, .StateBefore = originalDstState, .StateAfter = D3D12_RESOURCE_STATE_COPY_DEST}}};

    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 2, transitionToCopyBarriers);

    D3D12_TEXTURE_COPY_LOCATION srcLocation = {
        .pResource        = src->dx12.resource,
        .Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
        .SubresourceIndex = 0};
    D3D12_TEXTURE_COPY_LOCATION dstLocation = {
        .pResource       = dst->dx12.resource,
        .Type            = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
        .PlacedFootprint = srcFootprint};

    ID3D12GraphicsCommandList_CopyTextureRegion(cmdBuf->dx12.cmdList, &dstLocation, 0, 0, 0, &srcLocation, NULL);

    D3D12_RESOURCE_BARRIER restoreBarriers[2] = {
        {.Type          = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition = {
                .pResource   = src->dx12.resource,
                .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE,
                .StateAfter  = originalSrcState}},
        {.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .Transition = {.pResource = dst->dx12.resource, .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, .StateBefore = D3D12_RESOURCE_STATE_COPY_DEST, .StateAfter = originalDstState}}};

    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 2, restoreBarriers);
}

static void dx12_GenerateMipmapsFn(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_texture* texture)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(texture != NULL, "Texture cannot be NULL");

    RAZIX_RHI_LOG_ERROR("Mipmap generation not implemented for DX12 yet! (No native API like vulkan, needs shader emulation)");
    RAZIX_RHI_ABORT();
}

static void dx12_BlitTexture(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_texture* src, const rz_gfx_texture* dst)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(src != NULL, "Source texture cannot be NULL");
    RAZIX_RHI_ASSERT(dst != NULL, "Destination texture cannot be NULL");

    RAZIX_RHI_LOG_ERROR("Blit Texture not implemented for DX12 yet! (No native API like vulkan, needs shader emulation)");
    RAZIX_RHI_ABORT();
}

static void dx12_ResolveTexture(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_texture* src, const rz_gfx_texture* dst)
{
    RAZIX_RHI_ASSERT(cmdBuf != NULL, "Command buffer cannot be NULL");
    RAZIX_RHI_ASSERT(src != NULL, "Source texture cannot be NULL");
    RAZIX_RHI_ASSERT(dst != NULL, "Destination texture cannot be NULL");
    // TODO: Check if src is multi-sampled and dst is single sampled

    D3D12_RESOURCE_STATES originalSrcState = dx12_util_res_state_translate(src->resource.currentState);
    D3D12_RESOURCE_STATES originalDstState = dx12_util_res_state_translate(dst->resource.currentState);

    D3D12_RESOURCE_BARRIER transitionToResolveBarriers[2] = {
        {.Type          = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition = {
                .pResource   = src->dx12.resource,
                .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .StateBefore = originalSrcState,
                .StateAfter  = D3D12_RESOURCE_STATE_RESOLVE_SOURCE}},
        {.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .Transition = {.pResource = dst->dx12.resource, .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, .StateBefore = originalDstState, .StateAfter = D3D12_RESOURCE_STATE_RESOLVE_DEST}}};

    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 2, transitionToResolveBarriers);

    ID3D12GraphicsCommandList_ResolveSubresource(cmdBuf->dx12.cmdList,
        dst->dx12.resource,
        0,
        src->dx12.resource,
        0,
        dx12_util_rz_gfx_format_to_dxgi_format(src->resource.desc.textureDesc.format));

    D3D12_RESOURCE_BARRIER restoreBarriers[2] = {
        {.Type          = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition = {
                .pResource   = src->dx12.resource,
                .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .StateBefore = D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
                .StateAfter  = originalSrcState}},
        {.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .Transition = {.pResource = dst->dx12.resource, .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, .StateBefore = D3D12_RESOURCE_STATE_RESOLVE_DEST, .StateAfter = originalDstState}}};

    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 2, restoreBarriers);
}

static rz_gfx_syncpoint dx12_Signal(rz_gfx_syncobj* syncobj)
{
    RAZIX_RHI_ASSERT(syncobj != NULL, "Sync object cannot be NULL");
    RAZIX_RHI_ASSERT(syncobj->dx12.fence != NULL, "DX12 Fence in sync object cannot be NULL");

    rz_gfx_syncpoint signalSyncpoint = ++syncobj->waitSyncpoint;
    CHECK_HR(ID3D12CommandQueue_Signal(DX12Context.directQ, syncobj->dx12.fence, signalSyncpoint));
    return signalSyncpoint;
    return (rz_gfx_syncpoint) -1;
}

static void dx12_FlushGPUWork(rz_gfx_syncobj* frameSyncobj)
{
    RAZIX_RHI_ASSERT(frameSyncobj != NULL, "Frame sync object cannot be NULL");
    RAZIX_RHI_ASSERT(frameSyncobj->type == RZ_GFX_SYNCOBJ_TYPE_CPU || frameSyncobj->type == RZ_GFX_SYNCOBJ_TYPE_TIMELINE, "Frame sync object must be of type CPU or TIMELINE");

    rz_gfx_syncpoint signalValue = dx12_Signal(frameSyncobj);
    dx12_WaitOnPrevCmds(frameSyncobj);
}

static void dx12_ResizeSwapchain(rz_gfx_swapchain* sc, uint32_t width, uint32_t height)
{
    sc->width  = width;
    sc->height = height;

    for (uint32_t i = 0; i < sc->imageCount; ++i) {
        if (sc->backbuffers[i].dx12.resource) {
            ID3D12Resource_Release(sc->backbuffers[i].dx12.resource);
            sc->backbuffers[i].dx12.resource = NULL;
        }
    }

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {0};
    IDXGISwapChain4*     sc4           = sc->dx12.swapchain4;
    IDXGISwapChain4_GetDesc(sc4, &swapChainDesc);
    CHECK_HR(IDXGISwapChain4_ResizeBuffers(sc->dx12.swapchain4, sc->imageCount, sc->width, sc->height, dx12_util_rz_gfx_format_to_dxgi_format(RAZIX_SWAPCHAIN_FORMAT), swapChainDesc.Flags));

    dx12_util_update_swapchain_rtvs(sc);
}

//---------------------------------------------------------------------------------------------
// Jump table

rz_rhi_api dx12_rhi = {
    .GlobalCtxInit          = dx12_GlobalCtxInit,             // GlobalCtxInit
    .GlobalCtxDestroy       = dx12_GlobalCtxDestroy,          // GlobalCtxDestroy
    .CreateSyncobj          = dx12_CreateSyncobjFn,           // CreateSyncobj
    .DestroySyncobj         = dx12_DestroySyncobjFn,          // DestroySyncobj
    .CreateSwapchain        = dx12_CreateSwapchain,           // CreateSwapchain
    .DestroySwapchain       = dx12_DestroySwapchain,          // DestroySwapchain
    .CreateCmdPool          = dx12_CreateCmdPool,             // CreateCmdPool
    .DestroyCmdPool         = dx12_DestroyCmdPool,            // DestroyCmdPool
    .CreateCmdBuf           = dx12_CreateCmdBuf,              // CreateCmdBuf
    .DestroyCmdBuf          = dx12_DestroyCmdBuf,             // DestroyCmdBuf
    .CreateShader           = dx12_CreateShader,              // CreateShader
    .DestroyShader          = dx12_DestroyShader,             // DestroyShader
    .CreateRootSignature    = dx12_CreateRootSignature,       // CreateRootSignature
    .DestroyRootSignature   = dx12_DestroyRootSignature,      // DestroyRootSignature
    .CreatePipeline         = dx12_CreatePipeline,            // CreatePipeline
    .DestroyPipeline        = dx12_DestroyPipeline,           // DestroyPipeline
    .CreateTexture          = dx12_CreateTexture,             // CreateTexture
    .DestroyTexture         = dx12_DestroyTexture,            // DestroyTexture
    .CreateSampler          = dx12_CreateSampler,             // CreateSampler
    .DestroySampler         = dx12_DestroySampler,            // DestroySampler
    .CreateBuffer           = dx12_CreateBuffer,              // CreateBuffer
    .DestroyBuffer          = dx12_DestroyBuffer,             // DestroyBuffer
    .CreateResourceView     = dx12_CreateResourceView,        // CreateResourceView
    .DestroyResourceView    = dx12_DestroyResourceView,       // DestroyResourceView
    .CreateDescriptorHeap   = dx12_CreateDescriptorHeap,      // CreateDescriptorHeap
    .DestroyDescriptorHeap  = dx12_DestroyDescriptorHeap,     // DestroyDescriptorHeap
    .CreateDescriptorTable  = dx12_CreateDescriptorTable,     // CreateDescriptorTable
    .DestroyDescriptorTable = dx12_DestroyDescriptorTable,    // DestroyDescriptorTable

    .AcquireImage          = dx12_AcquireImage,             // AcquireImage
    .WaitOnPrevCmds        = dx12_WaitOnPrevCmds,           // WaitOnPrevCmds
    .Present               = dx12_Present,                  // Present
    .BeginCmdBuf           = dx12_BeginCmdBuf,              // BeginCmdBuf
    .EndCmdBuf             = dx12_EndCmdBuf,                // EndCmdBuf
    .SubmitCmdBuf          = dx12_SubmitCmdBuf,             // SubmitCmdBuf
    .BeginRenderPass       = dx12_BeginRenderPass,          // BeginRenderPass
    .EndRenderPass         = dx12_EndRenderPass,            // EndRenderPass
    .SetViewport           = dx12_SetViewport,              // SetViewport
    .SetScissorRect        = dx12_SetScissorRect,           // SetScissorRect
    .BindPipeline          = dx12_BindPipeline,             // BindPipeline
    .BindGfxRootSig        = dx12_BindGfxRootSig,           // BindGfxRootSig
    .BindComputeRootSig    = dx12_BindComputeRootSig,       // BindComputeRootSig
    .BindDescriptorTables  = dx12_BindDescriptorTables,     // BindDescriptorTable
    .BindDescriptorHeaps   = dx12_BindDescriptorHeaps,      // BindDescriptorHeaps
    .BindVertexBuffers     = dx12_BindVertexBuffers,        // BindVertexBuffers
    .BindIndexBuffer       = dx12_BindIndexBuffer,          // BindIndexBuffer
    .DrawAuto              = dx12_DrawAuto,                 // DrawAuto
    .DrawIndexedAuto       = dx12_DrawIndexedAuto,          // DrawIndexedAuto
    .Dispatch              = dx12_Dispatch,                 // Dispatch
    .DrawIndirect          = dx12_DrawIndirect,             // DrawIndirect
    .DrawIndexedIndirect   = dx12_DrawIndexedIndirect,      // DrawIndexedIndirect
    .DispatchIndirect      = dx12_DispatchIndirect,         // DispatchIndirect
    .UpdateDescriptorTable = dx12_UpdateDescriptorTable,    // UpdateDescriptorTable
    .UpdateConstantBuffer  = dx12_UpdateConstantBuffer,     // UpdateConstantBuffer
    .InsertImageBarrier    = dx12_InsertImageBarrier,       // InsertImageBarrier
    .InsertBufferBarrier   = dx12_InsertBufferBarrier,      // InsertBufferBarrier
    .InsertTextureReadback = dx12_InsertTextureReadback,    // InsertTextureReadback
    .InsertBufferReadback  = dx12_InsertBufferReadback,     // InsertBufferReadback
    .CopyBuffer            = dx12_CopyBuffer,               // CopyBuffer
    .CopyTexture           = dx12_CopyTexture,              // CopyTexture
    .CopyBufferToTexture   = dx12_CopyBufferToTexture,      // CopyBufferToTexture
    .CopyTextureToBuffer   = dx12_CopyTextureToBufferFn,    // CopyTextureToBuffer
    .GenerateMipmaps       = dx12_GenerateMipmapsFn,        // GenerateMipmaps
    .BlitTexture           = dx12_BlitTexture,              // BlitTexture
    .ResolveTexture        = dx12_ResolveTexture,           // ResolveTexture

    .SignalGPU       = dx12_Signal,             // Signal
    .FlushGPUWork    = dx12_FlushGPUWork,       // FlushGPUWork
    .ResizeSwapchain = dx12_ResizeSwapchain,    // ResizeSwapchain
};
