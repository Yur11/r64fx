#version 130

/* Vertex and texture 2d coordinates. */
in vec4 xyst;

/* Scale and shift. */
uniform vec4 sxsytxty = vec4(1.0, 1.0, 0.0, 0.0);


out vec2 frag_tex_coord;

void main()
{
    gl_Position = vec4(
        xyst[0] * sxsytxty[0] + sxsytxty[2],
        xyst[1] * sxsytxty[1] + sxsytxty[3],
        0.0,
        1.0
    );
    
    frag_tex_coord = vec2( xyst[2], xyst[3] );
}