/*
 * Razix Engine Shader File
 * Shader that can be used to render a grid, can be used in Editor and for visualization with a texture
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
layout(set = 0, binding = 1) uniform GridUniform
{
	vec3	CameraPos;
	float _padding;
	float	Scale;
	float	Resolution;
	float	MaxDistance;
	float _padding2;
}grid_ubo;

//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments 
layout(location = 0) out vec4 outFragColor;
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
	float lineResult = grid_ubo.Resolution - min(line, grid_ubo.Resolution);
	vec3 colour = vec3(0.2, 0.2, 0.2);

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

	outFragColor = Grid(divs) + Grid(divs / subdivisions);
	outFragColor.a *= clamp((decreaseDistance - distanceToCamera) / decreaseDistance, 0.0f, 1.0f);

	//outFragColor = vec4(fs_in.fragTexCoord, 0.0f, 1.0f);
}
//------------------------------------------------------------------------------
