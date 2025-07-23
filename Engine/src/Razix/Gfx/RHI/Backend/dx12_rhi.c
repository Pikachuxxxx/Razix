#include "dx12_rhi.h"

#include "Razix/Gfx/RHI/RHI.h"

#include <d3d12shader.h>
#include <d3dcompiler.h>

// TYpe friendly defines
#define DX12Context g_GfxCtx.dx12
#define DX12Device  g_GfxCtx.dx12.device10
#define CHECK_HR(x) dx12_util_check_hresult((x), __func__, __FILE__, __LINE__)
#ifndef RAZIX_GOLD_MASTER
    #define TAG_OBJECT(resource, name)                                              \
        if (resource) {                                                             \
            resource->lpVtbl->SetName(resource, dx12_util_string_to_lpcwstr(name)); \
        }
#else
TAG_OBJECT(resource, name)
#endif

//---------------------------------------------------------------------------------------------
// Internal types

typedef struct
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

//---------------------------------------------------------------------------------------------
// Util functions

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

static D3D12_RESOURCE_STATES dx12_util_res_state_translate(rz_gfx_resource_state state)
{
    switch (state) {
        case RZ_GFX_RESOURCE_STATE_RENDER_TARGET: return D3D12_RESOURCE_STATE_RENDER_TARGET;
        case RZ_GFX_RESOURCE_STATE_SHADER_READ: return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        case RZ_GFX_RESOURCE_STATE_COPY_SRC: return D3D12_RESOURCE_STATE_COPY_SOURCE;
        case RZ_GFX_RESOURCE_STATE_COPY_DST: return D3D12_RESOURCE_STATE_COPY_DEST;
        case RZ_GFX_RESOURCE_STATE_PRESENT: return D3D12_RESOURCE_STATE_PRESENT;
        case RZ_GFX_RESOURCE_STATE_DEPTH_WRITE: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
        case RZ_GFX_RESOURCE_STATE_GENERAL: return D3D12_RESOURCE_STATE_COMMON;
        default: return D3D12_RESOURCE_STATE_COMMON;
    }
}

static D3D12_DESCRIPTOR_RANGE_TYPE dx12_util_descriptor_type_to_range_type(rz_gfx_descriptor_type type)
{
    switch (type) {
        case RZ_GFX_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
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

static D3D12_INPUT_ELEMENT_DESC dx12_util_input_element_desc(rz_gfx_input_element element)
{
    D3D12_INPUT_ELEMENT_DESC dxElement = {0};
    dxElement.SemanticName             = element.pSemanticName;
    dxElement.SemanticIndex            = element.semanticIndex;
    dxElement.Format                   = dx12_util_rz_gfx_format_to_dxgi_format(element.format);
    dxElement.InputSlot                = element.inputSlot;
    dxElement.AlignedByteOffset        = element.alignedByteOffset;
    dxElement.InputSlotClass           = (element.inputClass == 1)
                                             ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA
                                             : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    dxElement.InstanceDataStepRate     = element.instanceStepRate;

    return dxElement;
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
        case RZ_GFX_POLYGON_MODE_TYPE_FILL: return D3D12_FILL_MODE_SOLID;
        case RZ_GFX_POLYGON_MODE_TYPE_LINE: return D3D12_FILL_MODE_WIREFRAME;
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
    D3D12_RENDER_TARGET_BLEND_DESC desc = {};
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

//---------------------------------------------------------------------------------------------
// Helper functions

static IDXGIAdapter4* dx12_select_best_adapter(IDXGIFactory7* factory, D3D_FEATURE_LEVEL min_feat_level)
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

static void dx12_query_features(dx12_ctx* ctx)
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

static void dx12_print_features(const D3D12FeatureCache* f)
{
    RAZIX_RHI_LOG_INFO("============ D3D12 Device Feature Info ============");

    RAZIX_RHI_LOG_INFO("Shader Model          : %u.%u",
        (f->shaderModel.HighestShaderModel >> 4) & 0xF,
        f->shaderModel.HighestShaderModel & 0xF);

    RAZIX_RHI_LOG_INFO("Root Signature        : v1.%d",
        f->rootSig.HighestVersion == D3D_ROOT_SIGNATURE_VERSION_1_1 ? 1 : 0);

    RAZIX_RHI_LOG_INFO("GPU Nodes             : %u", f->nodeCount);
    RAZIX_RHI_LOG_INFO("UMA                   : %s", f->isUMA ? "Yes" : "No");
    RAZIX_RHI_LOG_INFO("Cache-Coherent UMA    : %s", f->isCacheCoherentUMA ? "Yes" : "No");

    // D3D12_OPTIONS
    RAZIX_RHI_LOG_INFO("Conservative Raster   : %s", f->options.ConservativeRasterizationTier != D3D12_CONSERVATIVE_RASTERIZATION_TIER_NOT_SUPPORTED ? "Yes" : "No");
    RAZIX_RHI_LOG_INFO("Standard Swizzle      : %s", f->options.StandardSwizzle64KBSupported ? "Yes" : "No");
    RAZIX_RHI_LOG_INFO("Typed UAV Load ADDR64 : %s", f->options.TypedUAVLoadAdditionalFormats ? "Yes" : "No");

    // D3D12_OPTIONS1
    RAZIX_RHI_LOG_INFO("Wave Ops              : %s", f->options1.WaveOps ? "Yes" : "No");
    RAZIX_RHI_LOG_INFO("Wave Lane Count       : Min %u / Max %u", f->options1.WaveLaneCountMin, f->options1.WaveLaneCountMax);
    RAZIX_RHI_LOG_INFO("Int64 Shader Ops      : %s", f->options1.Int64ShaderOps ? "Yes" : "No");

    // D3D12_OPTIONS5
    RAZIX_RHI_LOG_INFO("Raytracing Tier       : Tier %d", f->options5.RaytracingTier);

    // VA support
    RAZIX_RHI_LOG_INFO("VA 64-bit Support     : %s", f->vaSupport.MaxGPUVirtualAddressBitsPerResource >= 44 ? "Yes" : "Partial/No");

    RAZIX_RHI_LOG_INFO("===================================================");
}

#ifdef _DEBUG
// Before Device
static void dx12_register_debug_interface(dx12_ctx* ctx)
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
static void dx12_d3d12_register_info_queue(dx12_ctx* ctx)
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

static void dx12_dxgi_register_info_queue(dx12_ctx* ctx)
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

static void dx12_track_dxgi_liveobjects(dx12_ctx* ctx)
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

static void dx12_destroy_debug_handles(dx12_ctx* ctx)
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

static void dx12_update_swapchain_rtvs(rz_gfx_swapchain* sc)
{
    sc->imageCount = RAZIX_MAX_SWAP_IMAGES_COUNT;
    ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(sc->dx12.rtvHeap, &sc->dx12.rtvHeapStart.cpu);

    for (uint32_t i = 0; i < sc->imageCount; ++i) {
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
        ID3D12Resource_SetName(d3dresource, dx12_util_string_to_lpcwstr("Swapchain Image"));

        // This is the only place where a RZ_RESOURCE is manually created, instead of using the RZResourceManager
        rz_gfx_texture texture     = {0};
        dx12_texture   dxtexture   = {0};
        texture.resource.pName     = "$SWAPCHAIN_IMAGE$";
        texture.resource.handle    = (rz_handle){i, i};
        texture.resource.viewHints = RZ_GFX_RESOURCE_VIEW_FLAG_RTV;
        dxtexture.resource         = d3dresource;
        dxtexture.state            = D3D12_RESOURCE_STATE_PRESENT;
        dxtexture.resView.rtv.cpu  = rtvHandle;
        texture.dx12               = dxtexture;
        texture.desc.height        = sc->height;
        texture.desc.width         = sc->width;
        texture.desc.arraySize     = 1;
        texture.desc.mipLevels     = 1;
        texture.desc.format        = RAZIX_SWAPCHAIN_FORMAT;
        texture.desc.textureType   = RZ_GFX_TEXTURE_TYPE_2D;
        sc->backbuffers[i]         = texture;

        TAG_OBJECT(d3dresource, "Swapchain Backbuffer Resource");
    }
}

static void dx12_create_backbuffers(rz_gfx_swapchain* sc)
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {0};
    rtvHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.NumDescriptors             = RAZIX_MAX_SWAP_IMAGES_COUNT;
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

    dx12_update_swapchain_rtvs(sc);
}

static void dx12_destroy_backbuffers(rz_gfx_swapchain* sc)
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

//---------------------------------------------------------------------------------------------
// Public API functions

static void dx12_GlobalCtxInit(void)
{
    RAZIX_RHI_LOG_INFO("Creating DXGI factory");

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

    DX12Context.adapter4 = dx12_select_best_adapter(DX12Context.factory7, DX12Context.featureLevel);
    if (DX12Context.adapter4 == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to select a suitable D3D12 adapter");
        return;
    }

#ifdef RAZIX_DEBUG
    // We register D3D12Debug interface before device create
    dx12_register_debug_interface(&DX12Context);
#endif

    // Create the device
    RAZIX_RHI_LOG_INFO("Creating D3D12 Device");
    CHECK_HR(D3D12CreateDevice((IUnknown*) DX12Context.adapter4, DX12Context.featureLevel, &IID_ID3D12Device10, (void**) &DX12Context.device10));
    if (DX12Context.device10 == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Device");
        return;
    }

#ifdef RAZIX_DEBUG
    // TODO: Use engine setting to enable/disable debugging
    // Register the D3D12 info queue after device creation
    dx12_d3d12_register_info_queue(&DX12Context);
    // Register the DXGI info queue
    dx12_dxgi_register_info_queue(&DX12Context);
    // Print the D3D12 features
    dx12_query_features(&DX12Context);
    dx12_print_features(&DX12Context.features);
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

    g_GraphicsFeatures.support.EnableVSync                  = false;
    g_GraphicsFeatures.support.TesselateTerrain             = false;
    g_GraphicsFeatures.support.SupportsBindless             = DX12Context.features.options.ResourceBindingTier >= D3D12_RESOURCE_BINDING_TIER_3;
    g_GraphicsFeatures.support.SupportsWaveIntrinsics       = true;
    g_GraphicsFeatures.support.SupportsShaderModel6         = DX12Context.features.shaderModel.HighestShaderModel >= D3D_SHADER_MODEL_6_0;
    g_GraphicsFeatures.support.SupportsNullIndexDescriptors = DX12Context.features.options5.SRVOnlyTiledResourceTier3;
    g_GraphicsFeatures.support.SupportsTimelineSemaphores   = true;
    g_GraphicsFeatures.MaxBindlessTextures                  = 4096;
    g_GraphicsFeatures.MinLaneWidth                         = DX12Context.features.options1.WaveLaneCountMin;
    g_GraphicsFeatures.MaxLaneWidth                         = DX12Context.features.options1.WaveLaneCountMax;
}

static void dx12_GlobalCtxDestroy(void)
{
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
    dx12_destroy_debug_handles(&DX12Context);
    dx12_track_dxgi_liveobjects(&DX12Context);
#endif
}

static void dx12_CreateSyncobjFn(void* where, rz_gfx_syncobj_type type)
{
    rz_gfx_syncobj* syncobj = (rz_gfx_syncobj*) where;

    // Create a fence for synchronization
    CHECK_HR(ID3D12Device10_CreateFence(DX12Device, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, (void**) &syncobj->dx12.fence));
    if (syncobj->dx12.fence == NULL) {
        RAZIX_RHI_LOG_ERROR("Failed to create D3D12 Fence");
        return;
    }

    if (type == RZ_GFX_SYNCOBJ_TYPE_CPU) {
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
    swapchain->imageCount  = RAZIX_MAX_SWAP_IMAGES_COUNT;

    DXGI_SWAP_CHAIN_DESC1 desc = {0};
    desc.Width                 = width;
    desc.Height                = height;
    desc.Format                = dx12_util_rz_gfx_format_to_dxgi_format(RAZIX_SWAPCHAIN_FORMAT);
    desc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount           = RAZIX_MAX_SWAP_IMAGES_COUNT;
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

    dx12_create_backbuffers(swapchain);
}

static void dx12_DestroySwapchain(rz_gfx_swapchain* sc)
{
    dx12_destroy_backbuffers(sc);

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
            if (desc->mesh.task.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_TASK;
            if (desc->mesh.mesh.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_MESH;
            if (desc->mesh.ps.bytecode) shader->shaderStageMask |= RZ_GFX_SHADER_STAGE_PIXEL;
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
    rootDesc.NumParameters             = desc->descriptorTableCount + desc->rootConstantCount;
    rootDesc.pParameters               = rootParams;

    for (uint32_t tableIdx = 0; tableIdx < desc->descriptorTableCount; tableIdx++) {
        const rz_gfx_descriptor_table_desc* pTableDesc = &desc->pDescriptorTablesDesc[tableIdx];
        RAZIX_RHI_ASSERT(pTableDesc != NULL, "Descriptor table cannot be NULL in root signature creation! (Root Signature creation)");

        D3D12_ROOT_PARAMETER* param = &rootParams[tableIdx];
        param->ParameterType        = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

        for (uint32_t rangeIdx = 0; rangeIdx < pTableDesc->descriptorCount; rangeIdx++) {
            RAZIX_RHI_ASSERT(rangeIdx < RAZIX_MAX_DESCRIPTOR_RANGES, "Too many descriptors in a table! [MAXLIMIT: %d] (Root Signature creation)", RAZIX_MAX_DESCRIPTOR_RANGES);

            const rz_gfx_descriptor* pDescriptor = &pTableDesc->pDescriptors[rangeIdx];
            RAZIX_RHI_ASSERT(pDescriptor != NULL, "Descriptor cannot be NULL in a descriptor table! (Root Signature creation)");
            RAZIX_RHI_ASSERT(pDescriptor->location.space == pTableDesc->tableIndex,
                "Descriptor space (%u) does not match table index (%u) in root signature creation! (Root Signature creation)",
                pDescriptor->location.space,
                pTableDesc->tableIndex);

            D3D12_DESCRIPTOR_RANGE* range            = &descriptorRanges[tableIdx][rangeIdx];
            range->RangeType                         = dx12_util_descriptor_type_to_range_type(pDescriptor->type);
            range->NumDescriptors                    = pDescriptor->memberCount;
            range->BaseShaderRegister                = pDescriptor->location.binding;
            range->RegisterSpace                     = pTableDesc->tableIndex;
            range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        }

        param->DescriptorTable.NumDescriptorRanges = pTableDesc->descriptorCount;
        param->DescriptorTable.pDescriptorRanges   = descriptorRanges[tableIdx];
    }

    for (uint32_t i = 0; i < desc->rootConstantCount; i++) {
        const rz_gfx_root_constant_desc* pRootConstantDesc = &desc->pRootConstantsDesc[i];
        RAZIX_RHI_ASSERT(pRootConstantDesc != NULL, "Root constant cannot be NULL in root signature creation! (Root Signature creation)");

        D3D12_ROOT_PARAMETER* param     = &rootParams[desc->descriptorTableCount + i];
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
    D3D12_INPUT_LAYOUT_DESC input_layout                                    = {0};
    input_layout.NumElements                                                = pShaderDesc->elementsCount;
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[RAZIX_MAX_VERTEX_ATTRIBUTES] = {0};
    for (uint32_t i = 0; i < input_layout.NumElements; i++) {
        rz_gfx_input_element elem = pShaderDesc->pElements[i];
        RAZIX_RHI_ASSERT(elem.format != RZ_GFX_FORMAT_UNDEFINED, "Input element format cannot be undefined");
        RAZIX_RHI_ASSERT(i < RAZIX_MAX_VERTEX_ATTRIBUTES, "Input element location exceeds maximum vertex attributes");

        // Convert the format to DXGI format
        inputElementDescs[i] = dx12_util_input_element_desc(elem);
    }
    input_layout.pInputElementDescs = pShaderDesc->elementsCount ? inputElementDescs : NULL;
    desc.InputLayout                = input_layout;

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
    depth.FrontFace                = (D3D12_DEPTH_STENCILOP_DESC){
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
    RAZIX_RHI_ASSERT(pShaderDesc->pipelineType == RZ_GFX_PIPELINE_TYPE_GRAPHICS, "Shader must be a graphics shader for this pipeline type! (Pipeline creation)");

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

//---------------------------------------------------------------------------------------------
// RHI

static void dx12_AcquireImage(rz_gfx_swapchain* sc)
{
    sc->currBackBufferIdx = IDXGISwapChain4_GetCurrentBackBufferIndex(sc->dx12.swapchain4);
}

static void dx12_WaitOnPrevCmds(const rz_gfx_syncobj* frameSyncobj, rz_gfx_syncpoint waitSyncPoint)
{
    rz_gfx_syncpoint completed = ID3D12Fence_GetCompletedValue(frameSyncobj->dx12.fence);

    if (completed < waitSyncPoint) {
        // Set the fence event and check for failure
        HRESULT hr = ID3D12Fence_SetEventOnCompletion(frameSyncobj->dx12.fence, waitSyncPoint, frameSyncobj->dx12.eventHandle);
        if (FAILED(hr)) {
            RAZIX_RHI_LOG_ERROR("[WAIT ERR] SetEventOnCompletion(%llu) failed -> 0x%08X", waitSyncPoint, hr);
        }

        // Wait for the event and log errors only
        DWORD result = WaitForSingleObject(frameSyncobj->dx12.eventHandle, INFINITE);
        if (result != WAIT_OBJECT_0) {
            RAZIX_RHI_LOG_ERROR("[WAIT ERR] WaitForSingleObject -> %s",
                result == WAIT_TIMEOUT ? "WAIT_TIMEOUT" : "WAIT_FAILED");
        }

        // Verify fence advanced
        rz_gfx_syncpoint new_completed = ID3D12Fence_GetCompletedValue(frameSyncobj->dx12.fence);
        if (new_completed < waitSyncPoint) {
            RAZIX_RHI_LOG_ERROR("[WAIT ERR] fence did not advance: completed=%llu, expected>=%llu", new_completed, waitSyncPoint);
        }
    }
}

static void dx12_Present(const rz_gfx_swapchain* sc)
{
    CHECK_HR(IDXGISwapChain4_Present(sc->dx12.swapchain4, 0, DXGI_PRESENT_ALLOW_TEARING));
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

static void dx12_SubmitCmdBuf(const rz_gfx_cmdbuf* cmdBuf)
{
    ID3D12GraphicsCommandList* cmdLists[] = {cmdBuf->dx12.cmdList};
    ID3D12CommandQueue_ExecuteCommandLists(DX12Context.directQ, 1, (ID3D12CommandList**) cmdLists);
}

static void dx12_BeginRenderPass(const rz_gfx_cmdbuf* cmdBuf, rz_gfx_renderpass renderPass)
{
    ID3D12GraphicsCommandList* cmdList = cmdBuf->dx12.cmdList;

    // Set Scissor and Rects
    D3D12_VIEWPORT vp = {
        .TopLeftX = 0,
        .TopLeftY = 0,
        .Width    = (FLOAT) RAZIX_X(renderPass.extents),
        .Height   = (FLOAT) RAZIX_Y(renderPass.extents),
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f};
    ID3D12GraphicsCommandList_RSSetViewports(cmdBuf->dx12.cmdList, 1, &vp);

    D3D12_RECT scissor = {
        .left   = 0,
        .top    = 0,
        .right  = (LONG) RAZIX_X(renderPass.extents),
        .bottom = (LONG) RAZIX_Y(renderPass.extents)};
    ID3D12GraphicsCommandList_RSSetScissorRects(cmdBuf->dx12.cmdList, 1, &scissor);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[RAZIX_MAX_RENDER_TARGETS] = {0};
    uint32_t                    rtvCount                             = renderPass.colorAttachmentsCount;

    for (uint32_t i = 0; i < rtvCount; ++i)
        rtvHandles[i] = renderPass.colorAttachments[i].pTexture->dx12.resView.rtv.cpu;

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = {0};
    bool                        hasDepth  = renderPass.depthAttachment.pTexture != NULL;

    if (hasDepth) {
        dsvHandle = renderPass.depthAttachment.pTexture->dx12.resView.dsv.cpu;
    }

    for (uint32_t i = 0; i < rtvCount; ++i) {
        if (renderPass.colorAttachments[i].clear) {
            float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
            memcpy(clearColor, renderPass.colorAttachments[i].clearColor.raw, sizeof(float) * 4);
            ID3D12GraphicsCommandList_ClearRenderTargetView(cmdList, rtvHandles[i], clearColor, 0, NULL);
        }
    }

    if (hasDepth && renderPass.depthAttachment.clear) {
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

// ...

static void dx12_BindPipeline(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_pipeline* pso)
{
    if (pso->resource.desc.pipelineDesc.type == RZ_GFX_PIPELINE_TYPE_GRAPHICS)
        ID3D12GraphicsCommandList_IASetPrimitiveTopology(cmdBuf->dx12.cmdList, pso->dx12.topology);
    ID3D12GraphicsCommandList_SetPipelineState(cmdBuf->dx12.cmdList, pso->dx12.pso);
}

static void dx12_DrawAuto(const rz_gfx_cmdbuf* cmdBuf, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    ID3D12GraphicsCommandList_DrawInstanced(cmdBuf->dx12.cmdList, vertexCount, instanceCount, firstVertex, firstInstance);
}

static void dx12_BindGfxRootSig(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_root_signature* rootSig)
{
    ID3D12GraphicsCommandList_SetGraphicsRootSignature(cmdBuf->dx12.cmdList, rootSig->dx12.rootSig);
}

static void dx12_BindComputeRootSig(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_root_signature* rootSig)
{
    ID3D12GraphicsCommandList_SetComputeRootSignature(cmdBuf->dx12.cmdList, rootSig->dx12.rootSig);
}

// ...

static void dx12_InsertImageBarrier(const rz_gfx_cmdbuf* cmdBuf, const rz_gfx_texture* texture, rz_gfx_resource_state beforeState, rz_gfx_resource_state afterState)
{
    D3D12_RESOURCE_BARRIER barrier = {
        .Type       = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags      = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition = {
            .pResource   = texture->dx12.resource,
            .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,    // TODO: use mip/layer combo
            .StateBefore = dx12_util_res_state_translate(beforeState),
            .StateAfter  = dx12_util_res_state_translate(afterState),
        }};
    ID3D12GraphicsCommandList_ResourceBarrier(cmdBuf->dx12.cmdList, 1, &barrier);
}

//---------------------------------------------------------------------------------------------
static rz_gfx_syncpoint dx12_Signal(const rz_gfx_syncobj* syncobj, rz_gfx_syncpoint* globalSyncPoint)
{
    if (syncobj && syncobj->dx12.fence) {
        ID3D12Fence*     fence           = syncobj->dx12.fence;
        rz_gfx_syncpoint signalSyncpoint = ++(*globalSyncPoint);
        CHECK_HR(ID3D12CommandQueue_Signal(DX12Context.directQ, fence, signalSyncpoint));
        return signalSyncpoint;
    }
    return (rz_gfx_syncpoint) -1;
}

static void dx12_FlushGPUWork(const rz_gfx_syncobj* frameSyncobj, rz_gfx_syncpoint* globalSyncpoint)
{
    rz_gfx_syncpoint signalValue = dx12_Signal(frameSyncobj, globalSyncpoint);
    dx12_WaitOnPrevCmds(frameSyncobj, signalValue);
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

    dx12_update_swapchain_rtvs(sc);
}

//---------------------------------------------------------------------------------------------

static void dx12_BeginFrame(rz_gfx_swapchain* sc, const rz_gfx_syncobj* frameSyncobj, rz_gfx_syncpoint* frameSyncPoints, rz_gfx_syncpoint* globalSyncPoint)
{
    (void) (globalSyncPoint);
    // This is reverse to what Vulkan does, we first wait for previous work to be done
    // and then acquire a new back buffer, because acquire is a GPU operation in vulkan,
    // here we just ask for index and wait on GPU until work is done and that back buffer is free to use
    dx12_AcquireImage(sc);
    uint32_t         frameIdx         = sc->currBackBufferIdx;
    rz_gfx_syncpoint currentSyncPoint = frameSyncPoints[frameIdx];
    dx12_WaitOnPrevCmds(frameSyncobj, currentSyncPoint);
}

static void dx12_EndFrame(const rz_gfx_swapchain* sc, const rz_gfx_syncobj* frameSyncobj, rz_gfx_syncpoint* frameSyncPoints, rz_gfx_syncpoint* globalSyncPoint)
{
    dx12_Present(sc);
    rz_gfx_syncpoint wait_value = dx12_Signal(frameSyncobj, globalSyncPoint);
    uint32_t         frame_idx  = sc->currBackBufferIdx;
    frameSyncPoints[frame_idx]  = wait_value;
}

//---------------------------------------------------------------------------------------------
// Jump table

rz_rhi_api dx12_rhi = {
    .GlobalCtxInit        = dx12_GlobalCtxInit,           // GlobalCtxInit
    .GlobalCtxDestroy     = dx12_GlobalCtxDestroy,        // GlobalCtxDestroy
    .CreateSyncobj        = dx12_CreateSyncobjFn,         // CreateSyncobj
    .DestroySyncobj       = dx12_DestroySyncobjFn,        // DestroySyncobj
    .CreateSwapchain      = dx12_CreateSwapchain,         // CreateSwapchain
    .DestroySwapchain     = dx12_DestroySwapchain,        // DestroySwapchain
    .CreateCmdPool        = dx12_CreateCmdPool,           // CreateCmdPool
    .DestroyCmdPool       = dx12_DestroyCmdPool,          // DestroyCmdPool
    .CreateCmdBuf         = dx12_CreateCmdBuf,            // CreateCmdBuf
    .DestroyCmdBuf        = dx12_DestroyCmdBuf,           // DestroyCmdBuf
    .CreateShader         = dx12_CreateShader,            // CreateShader
    .DestroyShader        = dx12_DestroyShader,           // DestroyShader
    .CreateRootSignature  = dx12_CreateRootSignature,     // CreateRootSignature
    .DestroyRootSignature = dx12_DestroyRootSignature,    // DestroyRootSignature
    .CreatePipeline       = dx12_CreatePipeline,          // CreatePipeline
    .DestroyPipeline      = dx12_DestroyPipeline,         // DestroyPipeline

    .AcquireImage       = dx12_AcquireImage,          // AcquireImage
    .WaitOnPrevCmds     = dx12_WaitOnPrevCmds,        // WaitOnPrevCmds
    .Present            = dx12_Present,               // Present
    .BeginCmdBuf        = dx12_BeginCmdBuf,           // BeginCmdBuf
    .EndCmdBuf          = dx12_EndCmdBuf,             // EndCmdBuf
    .SubmitCmdBuf       = dx12_SubmitCmdBuf,          // SubmitCmdBuf
    .BeginRenderPass    = dx12_BeginRenderPass,       // BeginRenderPass
    .EndRenderPass      = dx12_EndRenderPass,         // EndRenderPass
    .SetViewport        = dx12_SetViewport,           // SetViewport
    .SetScissorRect     = dx12_SetScissorRect,        // SetScissorRect
    .BindPipeline       = dx12_BindPipeline,          // BindPipeline
    .BindGfxRootSig     = dx12_BindGfxRootSig,        // BindGfxRootSig
    .BindComputeRootSig = dx12_BindComputeRootSig,    // BindComputeRootSig

    .DrawAuto           = dx12_DrawAuto,              // DrawAuto
    .InsertImageBarrier = dx12_InsertImageBarrier,    // InsertImageBarrier

    .SignalGPU       = dx12_Signal,             // Signal
    .FlushGPUWork    = dx12_FlushGPUWork,       // FlushGPUWork
    .ResizeSwapchain = dx12_ResizeSwapchain,    // ResizeSwapchain
    .BeginFrame      = dx12_BeginFrame,         // BeginFrame
    .EndFrame        = dx12_EndFrame,           // EndFrame
};