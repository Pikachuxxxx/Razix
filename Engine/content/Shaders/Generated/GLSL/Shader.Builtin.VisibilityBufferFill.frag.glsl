#version 450

layout(location = 3) flat in uint in_var_SV_InstanceID;
layout(location = 0) out vec4 out_var_SV_TARGET;

void main()
{
    uint _32 = (((uint(gl_PrimitiveID) << 0u) & 8388607u) | ((in_var_SV_InstanceID << 23u) & 2139095040u)) | 0u;
    out_var_SV_TARGET = vec4(float(_32 & 255u) * 0.0039215688593685626983642578125, float((_32 & 65280u) >> 8u) * 0.0039215688593685626983642578125, float((_32 & 16711680u) >> 16u) * 0.0039215688593685626983642578125, float((_32 & 4278190080u) >> 24u) * 0.0039215688593685626983642578125);
}

