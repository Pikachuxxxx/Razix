#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;

out VS_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
} vs_out;


uniform mat4 u_Model;
layout (std140) uniform VPMatrices
{
    mat4 u_View;
    mat4 u_Projection;
};

void main()
{
    vs_out.TexCoords    = texCoords;
    vs_out.Normal       = normal;//mat3(transpose(inverse(u_Model))) * normal;
    vs_out.FragPos      = vec3(u_Model * vec4(position, 1.0f));

    gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0f);
}
