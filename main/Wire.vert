#version 130

in vec4 vertex_data;

uniform vec4 scale_and_offset;

out float tex_coord;

void main()
{
    gl_Position = vec4(
        vertex_data[0] * scale_and_offset[0] + scale_and_offset[2], 
        vertex_data[1] * scale_and_offset[1] + scale_and_offset[3],
        0.0, 1.0
    );
    
    tex_coord = vertex_data[2];    
}