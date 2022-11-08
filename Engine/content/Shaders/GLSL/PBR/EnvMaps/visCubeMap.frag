#version 330 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
    vec4 FragPos;
    vec3 localPos;
} vs_in;

out vec4 FragColor;

uniform samplerCube envMap;

void main()
{
    vec3 envColor = texture(envMap, vs_in.localPos).rgb;

    envColor = envColor / (envColor + vec3(1.0f));
    envColor = pow(envColor, vec3(1.0f / 2.2f));

    FragColor = vec4(envColor, 1.0f);
}
