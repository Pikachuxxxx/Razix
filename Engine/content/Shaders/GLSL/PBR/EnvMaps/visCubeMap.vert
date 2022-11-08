#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT {
    vec3 normal;
    vec2 texCoords;
    vec4 FragPos;
    vec3 localPos;
} vs_out;

uniform mat4 u_Model = mat4(1.0f);
layout (std140) uniform VPMatrices
{
    mat4 u_View;
    mat4 u_Projection;
};

void main()
{
    vs_out.normal = mat3(transpose(inverse(u_Model))) * normal;
    vs_out.texCoords = texCoords;
    vs_out.FragPos = u_Model * vec4(position, 1.0f);
    vs_out.localPos = position;

    mat4 rotView = mat4(mat3(u_View));
    vec4 clipPos = u_Projection * rotView * vec4(vs_out.localPos, 1.0f);

    gl_Position = clipPos.xyww;
}
