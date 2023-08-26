#ifndef _OBJECT_DATA_GLSL_
#define _OBJECT_DATA_GLSL_
//----------------------------------------------------------------------------
#include <Common/ShaderInclude.Builtin.BindlessResources.glsl>
//----------------------------------------------------------------------------
struct ObjectData
{
    mat4 model;
    uint matIdx;
};

struct ObjectIndirectDrawArgs
{
};
//------------------------------------------------------------------------------
// Material Data
layout (std140, set = SET_IDX_BINDLESS_RESOURCES_START, binding = GLOBAL_BINDLESS_OBJECTDATA_STORAGE_BUFFERS_BINDING_IDX ) readonly buffer ObjectDataBuffer
{
    ObjectData objects[];
}objDataBuffer;
//----------------------------------------------------------------------------
#endif
 