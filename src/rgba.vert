#version 130

in vec2 position;

in vec4 color;

/* Scale and shift. */
uniform vec4 sxsytxty = vec4(1.0, 1.0, 0.0, 0.0);

out vec4 frag_color;

void main()
{
    gl_Position = vec4(
        position.x * sxsytxty[0] + sxsytxty[2],
        position.y * sxsytxty[1] + sxsytxty[3],
        0.0,
        1.0
    );

    frag_color = color;
}