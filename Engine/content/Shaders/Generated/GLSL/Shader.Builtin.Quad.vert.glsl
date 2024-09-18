#version 450

layout(location = 0) out vec2 out_var_TEXCOORD0;

void main()
{
    vec2 _27 = vec2(float((uint(gl_VertexIndex) << 1u) & 2u), float(uint(gl_VertexIndex) & 2u));
    out_var_TEXCOORD0 = _27;
    gl_Position = vec4((_27 * 2.0) - vec2(1.0), 0.0, 1.0);
}

