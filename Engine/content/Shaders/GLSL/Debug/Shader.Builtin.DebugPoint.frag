#version 450
/*
 * Razix Engine GLSL Fragment Shader File
 * Used to render a debug point
 */
// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
#extension GL_ARB_separate_shader_objects : enable
// This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
#extension GL_ARB_shading_language_420pack : enable

 //------------------------------------------------------------------------------
 // Vertex Input
 layout(location = 0) in VSOutput
 {
    vec4 fragColor;
    vec2 fragUV;
}fs_in;

//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------

float LinearizeDepth(float depth)
{
  float n = 0.1; // camera z near
  float f = 100.0; // camera z far
  float z = depth;
  return (2.0 * n) / (f + n - z * (f - n));	
}

void main()
{
	float distSq = dot(fs_in.fragUV, fs_in.fragUV);
	if (distSq > 1.0)
	{
		discard;
	}
	outFragColor = fs_in.fragColor;
}
//------------------------------------------------------------------------------