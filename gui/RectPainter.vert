#version 130

uniform vec4 geometry, tex_coord;
uniform vec4 scale_and_shift = vec4(1.0, 1.0, 0.0, 0.0);

in vec2 vertex_coord;

out vec2 fragment_tex_coord;

void main()
{
    gl_Position = vec4(
        (geometry[0] + (vertex_coord.x * geometry[2])) * scale_and_shift[0] + scale_and_shift[2], 
        (geometry[1] + (vertex_coord.y * geometry[3])) * scale_and_shift[1] + scale_and_shift[3], 
        0.0,
        1.0
    );
    
    fragment_tex_coord = vec2(
        tex_coord[0] + (vertex_coord.x * tex_coord[2]),
        tex_coord[1] + (vertex_coord.y * tex_coord[3])
    );
}