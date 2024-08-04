#version 450

const vec2 _30[3] = vec2[](vec2(-0.5), vec2(0.0, 0.5), vec2(0.5, -0.5));
const vec4 _34[3] = vec4[](vec4(1.0, 0.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0), vec4(0.0, 0.0, 1.0, 1.0));

layout(location = 0) out vec4 out_var_COLOR;
layout(location = 1) out vec2 out_var_TEXCOORD;

void main()
{
    gl_Position = vec4(_30[uint(gl_VertexIndex)], 0.0, 1.0);
    out_var_COLOR = _34[uint(gl_VertexIndex)];
    out_var_TEXCOORD = (_30[uint(gl_VertexIndex)] * 0.5) + vec2(0.5);
}

