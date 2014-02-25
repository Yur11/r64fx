#version 130

uniform float x, y, w, h;
uniform float glyph_width_coeff;

uniform vec4 sxsytxty = vec4(1.0, 1.0, 0.0, 0.0);

in vec2 vertex_coord;

out vec2 fragment_tex_coord;

void main()
{
    gl_Position = vec4(
        (x + (vertex_coord.x * w)) * sxsytxty[0] + sxsytxty[2], 
        (y + (vertex_coord.y * h)) * sxsytxty[1] + sxsytxty[3], 
        0.0, 
        1.0
    );
    
    fragment_tex_coord = vec2(vertex_coord.x * glyph_width_coeff, 1.0 - vertex_coord.y);
}