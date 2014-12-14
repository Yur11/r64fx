#version 130

in vec2 position;

in float ref_color;

in float tex_coord;

/* Scale and shift. */
uniform vec4 sxsytxty = vec4(1.0, 1.0, 0.0, 0.0);

out float frag_ref_color;

out float frag_tex_coord;

void main()
{
    gl_Position = vec4(
        position.x * sxsytxty[0] + sxsytxty[2],
        position.y * sxsytxty[1] + sxsytxty[3],
        0.0,
        1.0
    );
    
    frag_ref_color = ref_color;
    frag_tex_coord = tex_coord;
}