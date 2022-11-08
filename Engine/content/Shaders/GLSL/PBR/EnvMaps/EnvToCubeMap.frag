#version 330 core

in VS_OUT {
    vec3 normal;
    vec2 texCoords;
    vec4 FragPos;
    vec3 localPos;
} vs_in;

out vec4 FragColor;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    // color = vec4(vs_in.texCoords, 0.0f, 1.0f); // Testing UV Coordinates
    FragColor = vec4(texture(equirectangularMap, vs_in.texCoords).xxx, 1.0f);
    FragColor = vec4(vs_in.normal, 1.0f); // Testing Normal cooridinates
    // color = texture(texture_specular1, vs_in.texCoords);
    // color = vec4(lightColor, 1.0f);
    // color = vec4(1, 1, 1, 1);

    vec2 uv = SampleSphericalMap(normalize(vs_in.localPos));
    vec3 color = texture(equirectangularMap, uv).rgb;

    FragColor = vec4(color, 1.0f);
}
