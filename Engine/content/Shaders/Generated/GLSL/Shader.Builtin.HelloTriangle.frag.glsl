#version 450

layout(location = 0) in vec4 in_var_COLOR0;
layout(location = 0) out vec4 out_var_SV_TARGET;

void main()
{
    out_var_SV_TARGET = in_var_COLOR0;
}

