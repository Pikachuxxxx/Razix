#pragma once

#include <cstdint>

#include <glm/glm.hpp>

/**
 * Note:- Not an enum class because we need them as Int!!!! so the style guide is broke here on purpose
 * This for the render system 
 * for ex. for GI data, View proj data and Lighting data that will be defined by the rendering engine system data
 * we will have only set info for now as we implement if we need set info we'll see
 */
enum BindingTable_System : uint32_t
{
    BINDING_SET_SYSTEM_VIEW_PROJECTION   = 0,    // How to feed this to the Renderer? in terms of Include files order which is fucked up, should I make a header file to hold binding Material + Render System binding table infos
    BINDING_SET_SYSTEM_LIGHTING_DATA     = 1,
    BINDING_SET_SYSTEM_DEFERRED_LIGHTING = BINDING_SET_SYSTEM_LIGHTING_DATA,
};

// TODO: Add the ViewProjection (+ maybe Light) as system UBOs and perform static Initialization for all the Renderers
// TODO!!!: [High Priority!!!] Combine View Projection into a single matrix
struct ModelViewProjectionSystemUBOData
{
    alignas(16) glm::mat4 model      = glm::mat4(1.0f);
    alignas(16) glm::mat4 view       = glm::mat4(1.0f);
    alignas(16) glm::mat4 projection = glm::mat4(1.0f);
};