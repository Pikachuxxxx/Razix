#ifndef _BINDLESS_RESOURCES_GLSL_
#define _BINDLESS_RESOURCES_GLSL_

// Check C:\Dev\Game Engines\Razix\Engine\src\Razix\Graphics\Renderers\RZSystemBinding.h for Binding Table

#define MAX_DESCRIPTORS 1024

#define BINDLESS_RESOURCES_START_IDX                            1 // @ 0 we have FrameData
#define GLOBAL_BINDLESS_TEXTURES_2D_BINDING_IDX                 BINDLESS_RESOURCES_START_IDX
#define GLOBAL_BINDLESS_TEXTURES_3D_BINDING_IDX                 BINDLESS_RESOURCES_START_IDX
#define GLOBAL_BINDLESS_TEXTURES_CUBEMAP_BINDING_IDX            BINDLESS_RESOURCES_START_IDX
//#define GLOBAL_BINDLESS_STORAGE_TEXTURES_BINDING_IDX            BINDLESS_RESOURCES_START_IDX + 3
#define GLOBAL_BINDLESS_MATERIAL_STORAGE_BUFFERS_BINDING_IDX    BINDLESS_RESOURCES_START_IDX + 3
#define GLOBAL_BINDLESS_OBJECTDATA_STORAGE_BUFFERS_BINDING_IDX  BINDLESS_RESOURCES_START_IDX + 4

#ifndef SET_IDX_BINDLESS_RESOURCES_START
#define SET_IDX_BINDLESS_RESOURCES_START                        1
#endif

//------------------------------------------------------------------------------
// Texture Pools
layout ( set = SET_IDX_BINDLESS_RESOURCES_START, binding = GLOBAL_BINDLESS_TEXTURES_2D_BINDING_IDX ) uniform sampler2D global_textures_2d[MAX_DESCRIPTORS];
layout ( set = SET_IDX_BINDLESS_RESOURCES_START, binding = GLOBAL_BINDLESS_TEXTURES_3D_BINDING_IDX ) uniform sampler3D global_textures_3d[MAX_DESCRIPTORS];
layout ( set = SET_IDX_BINDLESS_RESOURCES_START, binding = GLOBAL_BINDLESS_TEXTURES_CUBEMAP_BINDING_IDX ) uniform samplerCube global_textures_cubemap[MAX_DESCRIPTORS];
//------------------------------------------------------------------------------
//layout ( set = SET_IDX_BINDLESS_RESOURCES_START, binding = GLOBAL_BINDLESS_STORAGE_TEXTURES_BINDING_IDX ) uniform readonly image2D global_storage_textures[MAX_DESCRIPTORS];
//------------------------------------------------------------------------------
//layout ( set = SET_IDX_BINDLESS_RESOURCES_START, binding = GLOBAL_BINDLESS_UNIFORM_BUFFERS_BINDING_IDX ) uniform image2D global_uniforms[];
//------------------------------------------------------------------------------

// Bindless support
// Enable non uniform qualifier extension
#extension GL_EXT_nonuniform_qualifier : enable
// Usage : global_textures_xx[nonuniformEXT(idx)]

#endif