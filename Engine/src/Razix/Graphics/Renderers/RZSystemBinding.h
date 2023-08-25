#pragma once

#include <cstdint>

#include <glm/glm.hpp>

/**
 * Note:- Not an enum class because we need them as Int!!!! so the style guide is broke here on purpose
 * This for the render system 
 * for ex. for GI data, View proj data and Lighting data that will be defined by the rendering engine system data
 * we will have only set info for now as we implement if we need set info we'll see
 */

/**
 * All the sets at index 0 is for View Projection UBO (no samplers)
 * Set index 1 is for Lighting Buffers only (no textures)
 * Set index 2 is for Material buffer data + Textures (How do we fit sampler and fill the data it's a bit of messy for now)
 * other sets will be allocated for GI, Decals etc up to 32/16
 */

// TODO: Use this file in the shaders
enum BindingTable_System : u32
{
    // Set Indices
    SET_IDX_FRAME_DATA               = 0,
    SET_IDX_SYSTEM_START             = SET_IDX_FRAME_DATA,
    SET_IDX_BINDLESS_RESOURCES_START = 1,
    SET_IDX_MATERIAL_DATA            = 2,
    SET_IDX_LIGHTING_DATA            = 3,
    SET_IDX_SHADOW_DATA              = 4,
    SET_IDX_PBR_DATA                 = 5,
    SET_IDX_SKYBOX_DATA              = SET_IDX_MATERIAL_DATA,
    SET_IDX_USER_DATA_START          = 4,
    SET_IDX_USER_DATA_SLOT_0         = SET_IDX_USER_DATA_START,

    BINDING_IDX_BINDLESS_RESOURCES_START                     = 0,
    BINDING_IDX_GLOBAL_BINDLESS_TEXTURES_2D_BINDING_IDX      = BINDING_IDX_BINDLESS_RESOURCES_START,
    BINDING_IDX_GLOBAL_BINDLESS_TEXTURES_3D_BINDING_IDX      = BINDING_IDX_BINDLESS_RESOURCES_START + 1,
    BINDING_IDX_GLOBAL_BINDLESS_TEXTURES_CUBEMAP_BINDING_IDX = BINDING_IDX_BINDLESS_RESOURCES_START + 2,
    BINDING_IDX_GLOBAL_BINDLESS_STORAGE_TEXTURES_BINDING_IDX = BINDING_IDX_BINDLESS_RESOURCES_START + 3,
    BINDING_IDX_GLOBAL_BINDLESS_UNIFORM_BUFFERS_BINDING_IDX  = BINDING_IDX_BINDLESS_RESOURCES_START + 4,
    BINDING_IDX_FRAME_DATA                                   = 0,
    BINDING_IDX_MAT_PROPS                                    = 0,
    BINDING_IDX_MAT_SAMPLERS                                 = 1,
    BINDING_IDX_LIGHTING_DATA                                = 0,
    BINDING_IDX_DEFERRED_LIGHTING                            = BINDING_IDX_LIGHTING_DATA,
    BINDING_IDX_SHADOW_MAP                                   = 0,
    BINDING_IDX_CSM_SHADOW_MAP                               = 0,
    BINDING_IDX_SHADOW_MATRIX                                = 1,
    BINDING_IDX_CSM_SHADOW_MATRICES                          = 1,
    BINDING_IDX_PBR_IRRADIANCE_MAP                           = 0,
    BINDING_IDX_PBR_PREFILTERED_MAP                          = 1,
    BINDING_IDX_PBR_BRDF_LUT                                 = 2,
    BINDING_IDX_SKYBOX_ENVMAP                                = 0,
};

/* lighting model texture binding slots */
enum TextureBindingTable : u32
{
    BINDING_IDX_TEX_ALBEDO    = 1,
    BINDING_IDX_TEX_NORMAL    = 2,
    BINDING_IDX_TEX_METALLLIC = 3,
    BINDING_IDX_TEX_ROUGHNESS = 4,
    BINDING_IDX_TEX_SPECULAR  = 5,
    BINDING_IDX_TEX_EMISSIVE  = 6,
    BINDING_IDX_TEX_AO        = 7,
};