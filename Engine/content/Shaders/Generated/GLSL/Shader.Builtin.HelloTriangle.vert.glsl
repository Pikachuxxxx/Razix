#version 450

const vec2 _27[3] = vec2[](vec2(-0.5), vec2(0.0, 0.5), vec2(0.5, -0.5));
const vec4 _31[3] = vec4[](vec4(1.0, 0.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0), vec4(0.0, 0.0, 1.0, 1.0));

layout(location = 0) out vec4 out_var_COLOR0;

void main()
{
    out_var_COLOR0 = _31[uint(gl_VertexIndex)];
    gl_Position = vec4(_27[uint(gl_VertexIndex)], 0.0, 1.0);
}

