/*
 * Razix Engine HLSL Compute Shader File
 * Visibility Buffer Shade: To unpack the vertex attributes from the Visibility Buffer and shader the geometry surface
 */
//------------------------------------------------------------------------------
#include "../../../ShaderCommon/ShaderInclude.Builtin.ShaderLangCommon.h"
//------------------------------------------------------------------------------
#include "../../Math/ShaderInclude.Builtin.PackingUtils.h"

#include "ShaderInclude.Builtin.VisibilityBufferCommon.h"
//------------------------------------------------------------------------------
// Buffers
//------------------------------------------------------------------------------

[numthreads (VB_SHADE_TILE_SZ, VB_SHADE_TILE_SZ, 1)]
void CS_MAIN(uint3 globalThreadID: SV_DispatchThreadID, uint3 tileThreadID: SV_GroupThreadID, uint3 groupID: SV_GroupID) 
{
    
    GroupMemoryBarrierWithGroupSync();
}