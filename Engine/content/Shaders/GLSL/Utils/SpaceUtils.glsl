#ifndef _SPACE_UTILS_GLSL_
#define _SPACE_UTILS_GLSL_

#ifndef _FRAME_DATA_GLSL_
#  error "Common/ShaderInclude.Builtin.FrameData.glsl required"
#endif

#define ORIGIN_UPPER_LEFT 0

vec4 worldToView(vec4 v) { return FrameData.info.camera.view * v; }
vec4 viewToWorld(vec4 v) { return FrameData.info.camera.inversedView * v; }

vec3 viewToClip(vec4 v) {
  vec4 clip = FrameData.info.camera.projection * v;
  return clip.xyz / clip.w;
}
vec3 clipToView(vec4 v) {
  const vec4 view = FrameData.info.camera.inversedProjection * v;
  return view.xyz / view.w;
}

vec3 screenToView(vec4 screen) {
  vec2 texCoord = screen.xy / vec2(FrameData.resolution);
#if ORIGIN_UPPER_LEFT
  texCoord.y = 1.0 - texCoord.y;
#endif
  return clipToView(vec4(texCoord * 2.0 - 1.0, screen.zw));
}

#endif
