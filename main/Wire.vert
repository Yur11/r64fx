#version 130

in vec4 vertex_data;

out float tex_coord;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * vec4(
        vertex_data[0], vertex_data[1], 0.0, 1.0
    );
    
    tex_coord = vertex_data[2];    
}