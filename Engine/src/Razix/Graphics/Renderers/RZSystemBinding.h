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

enum BindingTable_System : uint32_t
{
    BINDING_SET_SYSTEM_VIEW_PROJECTION   = 0,    // How to feed this to the Renderer? in terms of Include files order which is fucked up, should I make a header file to hold binding Material + Render System binding table infos
    BINDING_SET_SYSTEM_MAT_PROPS         = 1,
    BINDING_SET_SYSTEM_MAT_SAMPLERS      = 1,
    BINDING_SET_SYSTEM_LIGHTING_DATA     = 2,
    BINDING_SET_SYSTEM_DEFERRED_LIGHTING = BINDING_SET_SYSTEM_LIGHTING_DATA,
};

/* lighting model texture binding slots */
enum TextureBindingTable : uint32_t
{
    TEX_BINDING_IDX_ALBEDO    = 1,
    TEX_BINDING_IDX_NORMAL    = 2,
    TEX_BINDING_IDX_METALLLIC = 3,
    TEX_BINDING_IDX_ROUGHNESS = 4,
    TEX_BINDING_IDX_SPECULAR  = 5,
    TEX_BINDING_IDX_EMISSIVE  = 6,
    TEX_BINDING_IDX_AO        = 7,
};

// TODO: Add the ViewProjection (+ maybe Light) as system UBOs and perform static Initialization for all the Renderers
// TODO!!!: [High Priority!!!] Combine View Projection into a single matrix
struct ModelViewProjectionSystemUBOData
{
    alignas(16) glm::mat4 model          = glm::mat4(1.0f);
    alignas(16) glm::mat4 viewProjection = glm::mat4(1.0f);
};