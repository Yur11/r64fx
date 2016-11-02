#version 130

/* Scale and shift. */
uniform vec4 sxsytxty = vec4(1.0, 1.0, 0.0, 0.0);

in vec2 position;

void main()
{
    gl_Position = vec4(
        position.x * sxsytxty[0] + sxsytxty[2],
        position.y * sxsytxty[1] + sxsytxty[3],
        0.0,
        1.0
    );
}
