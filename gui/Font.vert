#version 130

uniform float x, y, w, h;
uniform float glyph_width_coeff;

in vec2 vertex_coord;

out vec2 fragment_tex_coord;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * vec4(
        x + (vertex_coord.x * w), 
        y + (vertex_coord.y * h), 
        0.0, 
        1.0
    );
    
    fragment_tex_coord = vec2(vertex_coord.x * glyph_width_coeff, 1.0 - vertex_coord.y);
}