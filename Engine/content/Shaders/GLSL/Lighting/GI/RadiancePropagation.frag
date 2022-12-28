/*
 * Razix Engine Shader File
 * RadiancePropagation.frag
 */
 #version 450 core
  // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable
 //------------------------------------------------------------------------------
#include <Lighting/GI/LPV.glsl>
//------------------------------------------------------------------------------
layout(location = 0) in FragData 
{ 
    flat ivec3 cellIndex;
}fs_in;

layout(set = 0, binding = 1) uniform sampler3D t_SH_R;
layout(set = 0, binding = 2) uniform sampler3D t_SH_G;
layout(set = 0, binding = 3) uniform sampler3D t_SH_B;

//------------------------------------------------------------------------------
const vec2 kSide[4] = {
  vec2( 1.0,  0.0),
  vec2( 0.0,  1.0),
  vec2(-1.0,  0.0),
  vec2( 0.0, -1.0)
};
//------------------------------------------------------------------------------
// orientation = [ right | up | forward ] = [ x | y | z ]
vec3 getEvalSideDirection(int index, mat3 orientation) {
  const float smallComponent = 0.4472135; // 1 / sqrt(5)
  const float bigComponent = 0.894427;    // 2 / sqrt(5)

  const vec2 side = kSide[index];
  return orientation *
         vec3(side.x * smallComponent, side.y * smallComponent, bigComponent);
}
vec3 getReprojSideDirection(int index, mat3 orientation) {
  const vec2 side = kSide[index];
  return orientation * vec3(side.x, side.y, 0);
}

// orientation = [ right | up | forward ] = [ x | y | z ]
const mat3 kNeighbourOrientations[6] = {
  mat3(1, 0, 0, 0, 1, 0, 0, 0, 1),   // Z+
  mat3(-1, 0, 0, 0, 1, 0, 0, 0, -1), // Z-
  mat3(0, 0, 1, 0, 1, 0, -1, 0, 0),  // X+
  mat3(0, 0, -1, 0, 1, 0, 1, 0, 0),  // X-
  mat3(1, 0, 0, 0, 0, 1, 0, -1, 0),  // Y+
  mat3(1, 0, 0, 0, 0, -1, 0, 1, 0)   // Y-
};

SHcoeffs gatherContributions(ivec3 cellIndex) {
  SHcoeffs contribution = {vec4(0.0), vec4(0.0), vec4(0.0)};

  for (int neighbour = 0; neighbour < 6; ++neighbour) {
    const mat3 orientation = kNeighbourOrientations[neighbour];
    const vec3 mainDirection = orientation * vec3(0.0, 0.0, 1.0);

    const ivec3 neighbourIndex = cellIndex - ivec3(mainDirection);
    const SHcoeffs neighbourCoeffs = {
      texelFetch(t_SH_R, neighbourIndex, 0),
      texelFetch(t_SH_G, neighbourIndex, 0),
      texelFetch(t_SH_B, neighbourIndex, 0)
    };

    const float kDirectFaceSubtendedSolidAngle = 0.4006696846 / PI;

    const vec4 mainDirectionCosineLobeSH = SH_evaluateCosineLobe(mainDirection);
    const vec4 mainDirectionSH = SH_evaluate(mainDirection);
    contribution.red += kDirectFaceSubtendedSolidAngle *
                        dot(neighbourCoeffs.red, mainDirectionSH) *
                        mainDirectionCosineLobeSH;
    contribution.green += kDirectFaceSubtendedSolidAngle *
                          dot(neighbourCoeffs.green, mainDirectionSH) *
                          mainDirectionCosineLobeSH;
    contribution.blue += kDirectFaceSubtendedSolidAngle *
                         dot(neighbourCoeffs.blue, mainDirectionSH) *
                         mainDirectionCosineLobeSH;

    const float kSideFaceSubtendedSolidAngle = 0.4234413544 / PI;

    for (int sideFace = 0; sideFace < 4; ++sideFace) {
      const vec3 evalDirection = getEvalSideDirection(sideFace, orientation);
      const vec3 reprojDirection =
        getReprojSideDirection(sideFace, orientation);

      const vec4 reprojDirectionCosineLobeSH =
        SH_evaluateCosineLobe(reprojDirection);
      const vec4 evalDirectionSH = SH_evaluate(evalDirection);

      contribution.red += kSideFaceSubtendedSolidAngle *
                          dot(neighbourCoeffs.red, evalDirectionSH) *
                          reprojDirectionCosineLobeSH;
      contribution.green += kSideFaceSubtendedSolidAngle *
                            dot(neighbourCoeffs.green, evalDirectionSH) *
                            reprojDirectionCosineLobeSH;
      contribution.blue += kSideFaceSubtendedSolidAngle *
                           dot(neighbourCoeffs.blue, evalDirectionSH) *
                           reprojDirectionCosineLobeSH;
    }
  }

  return contribution;
}
//------------------------------------------------------------------------------
layout(location = 0) out vec4 AccumulatedSH_R;
layout(location = 1) out vec4 AccumulatedSH_G;
layout(location = 2) out vec4 AccumulatedSH_B;
//------------------------------------------------------------------------------
void main() {
  const SHcoeffs c = gatherContributions(fs_in.cellIndex);

  AccumulatedSH_R = c.red;
  AccumulatedSH_G = c.green;
  AccumulatedSH_B = c.blue;
}
