/*
 * Razix Engine Shader File
 * RadiancePropagation.geom
 */
 #version 450 core
  // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable

layout(points) in;
layout(points, max_vertices = 1) out;

layout(location = 0) in VertexData 
{ 
    flat ivec3 cellIndex;
}gs_in[];

layout(location = 0) out FragData
{
    flat ivec3 cellIndex;
}gs_out;

void main() {
    gl_Position = gl_in[0].gl_Position;
    gl_PointSize = gl_in[0].gl_PointSize;
    gl_Layer = gs_in[0].cellIndex.z;

    gs_out.cellIndex = gs_in[0].cellIndex;

    EmitVertex();
    EndPrimitive();
}
