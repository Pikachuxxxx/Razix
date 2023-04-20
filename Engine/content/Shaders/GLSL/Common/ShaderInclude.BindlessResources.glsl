#ifndef _BINDLESS_RESOURCES_GLSL_
#define _BINDLESS_RESOURCES_GLSL_

#define BINDLESS_RESOURCES_START_IDX            10
#define GLOBAL_TEXTURES_BINDING_IDX             BINDLESS_RESOURCES_START_IDX
#define GLOBAL_STORAGE_TEXTURES_BINDING_IDX     (BINDLESS_RESOURCES_START_IDX + 1)
#define GLOBAL_UNIFORM_BUFFERS_BINDING_IDX      (BINDLESS_RESOURCES_START_IDX + 2)

//------------------------------------------------------------------------------
// Alias textures to use the same binding point, as bindless texture is shared
// between all kind of textures: 1d, 2d, 3d and cubeMap
layout ( set = 1, binding = GLOBAL_TEXTURES_BINDING_IDX ) uniform sampler3D global_textures_3d[];
layout ( set = 1, binding = GLOBAL_TEXTURES_BINDING_IDX ) uniform sampler2D global_textures[];
layout ( set = 1, binding = GLOBAL_TEXTURES_BINDING_IDX ) uniform samplerCube global_textures_cubemap[];
//------------------------------------------------------------------------------
layout ( set = 1, binding = GLOBAL_STORAGE_TEXTURES_BINDING_IDX ) uniform image2D global_storage_textures[];
//------------------------------------------------------------------------------
//layout ( set = 1, binding = GLOBAL_UNIFORM_BUFFERS_BINDING_IDX ) uniform image2D global_uniforms[];
//------------------------------------------------------------------------------

// Bindless support
// Enable non uniform qualifier extension
#extension GL_EXT_nonuniform_qualifier : enable

#endif