#ifndef _SPACE_UTILS_GLSL_
#define _SPACE_UTILS_GLSL_

#ifndef _FRAME_DATA_GLSL_
#  error "Common/ShaderInclude.FrameData.glsl required"
#endif

#define ORIGIN_UPPER_LEFT 0

vec4 worldToView(vec4 v) { return u_Frame.camera.view * v; }
vec4 viewToWorld(vec4 v) { return u_Frame.camera.inversedView * v; }

vec3 viewToClip(vec4 v) {
  vec4 clip = u_Frame.camera.projection * v;
  return clip.xyz / clip.w;
}
vec3 clipToView(vec4 v) {
  const vec4 view = u_Frame.camera.inversedProjection * v;
  return view.xyz / view.w;
}

vec3 screenToView(vec4 screen) {
  vec2 texCoord = screen.xy / vec2(u_Frame.resolution);
#if ORIGIN_UPPER_LEFT
  texCoord.y = 1.0 - texCoord.y;
#endif
  return clipToView(vec4(texCoord * 2.0 - 1.0, screen.zw));
}

#endif
