/*
 * Razix Engine Shader File
 * Shader that can be used to render a grid, can be used in Editor and for visualization
 */
 #version 450
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable

 //------------------------------------------------------------------------------
 // Vertex Input
 layout(location = 0) in VSOutput
 {
    vec3 fragPosition;
    vec2 fragTexCoord;
 }fs_in;
 //------------------------------------------------------------------------------
 // Fragment Shader Stage Uniforms
layout(set = 0, binding = 1) uniform sampler2D texSampler;
layout(set = 0, binding = 2) uniform GridUniform
{
	vec4  CameraPos;
	float Scale;
	float Res;
	float MaxDistance;
}grid_ubo;

            //float m_GridRes = 1.0f;
            //float m_GridSize = 1.0f;
            //float m_MaxDistance = 600.0f;
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments 
layout(location = 0) out vec4 outColor;
//------------------------------------------------------------------------------
// Constants and defines
const float step = 100.0f;
const float subdivisions = 10.0f;
//------------------------------------------------------------------------------
// Utility methods
vec4 Grid(float divisions)
{
	vec2 coord = fs_in.fragTexCoord.xy * divisions;

	vec2 grid = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
	float line = min(grid.x, grid.y);
	float lineResult = grid_ubo.Res - min(line, grid_ubo.Res);
	vec3 colour = vec3(0.3, 0.3, 0.3);

	return vec4(vec3(lineResult) * colour, 0.1 * lineResult);
}
//------------------------------------------------------------------------------
// main
void main()
{

	vec3 pseudoViewPos = vec3(grid_ubo.CameraPos.x, fs_in.fragPosition.y, grid_ubo.CameraPos.z);
	float distanceToCamera = max(distance(fs_in.fragPosition, pseudoViewPos) - abs(grid_ubo.CameraPos.y), 0);

	float divs = grid_ubo.Scale / pow(2, round((abs(grid_ubo.CameraPos.y) - step / subdivisions) / step));

	float decreaseDistance = grid_ubo.MaxDistance * 1.5;

	outColor = Grid(divs) + Grid(divs / subdivisions);
	outColor.a *= clamp((decreaseDistance - distanceToCamera) / decreaseDistance, 0.0f, 1.0f);
}
//------------------------------------------------------------------------------
