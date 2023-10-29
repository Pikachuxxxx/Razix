/*
 * Razix Engine Shader File
 * Default Fragment Shader that can be used for rendering basic geometry with versampleColor colors and use a sampleColorture as well
 */
 #version 450
 // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_separate_shader_objects.txt Read this for why this extension is enables for all glsl shaders
 #extension GL_ARB_separate_shader_objects : enable
 // This extension is enabled for additional glsl features introduced after 420 check https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_420pack.txt for more details
 #extension GL_ARB_shading_language_420pack : enable
//------------------------------------------------------------------------------
 #define MAXCOLOR 15.0 // Constants for max color (15 since we start from 0), number of colors per channel and lookup texture width and height
 #define COLORS 16.0
 #define WIDTH 256.0
 #define HEIGHT 16.0
 //------------------------------------------------------------------------------
// VersampleColor Input
layout(location = 0) in VSOutput
{
    vec2 uv;
}fs_in;

//------------------------------------------------------------------------------
// Fragment Shader Stage Uniforms
layout(set = 0, binding = 0) uniform sampler2D SceneHDRSource;
layout(set = 0, binding = 1) uniform sampler2D NeutralLUT;
//------------------------------------------------------------------------------
// Output from Fragment Shader or Output to Framebuffer attachments
layout(location = 0) out vec4 outFragColor;
//------------------------------------------------------------------------------

void main()
{
    // [Source] : https://defold.com/tutorials/grading/

    // Sample a pixel color
    vec4 px = texture(SceneHDRSource, fs_in.uv); 

    // Calculate which cell to read color from based on the blue channel value of color
    float cell = px.b * MAXCOLOR; 
    // Calculate the two adjacent cells to read from
    float cell_l = floor(cell); 
    float cell_h = ceil(cell);

    // Calculate half pixel offsets so we read from pixel centers
    float half_px_x = 0.5 / WIDTH;
    float half_px_y = 0.5 / HEIGHT;
    float r_offset = half_px_x + px.r / COLORS * (MAXCOLOR / COLORS);
    float g_offset = half_px_y + px.g * (MAXCOLOR / COLORS);

    // Calculate two separate lookup positions, one for each cell
    vec2 lut_pos_l = vec2(cell_l / COLORS + r_offset, g_offset);
    vec2 lut_pos_h = vec2(cell_h / COLORS + r_offset, g_offset);

    // Sample the two colors from the cell positions
    vec4 graded_color_l = texture(NeutralLUT, lut_pos_l);
    vec4 graded_color_h = texture(NeutralLUT, lut_pos_h);

    // Mix the colors linearly according to the fraction of cell, which is the scaled blue color value
    vec4 graded_color = mix(graded_color_l, graded_color_h, fract(cell));

    outFragColor = vec4(graded_color.rgb, 1.0f);
}
//------------------------------------------------------------------------------