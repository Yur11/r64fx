#version 130

in vec2 frag_tex_coord;

uniform vec4 color;
uniform sampler2D sampler;
uniform int texturing_mode = 1;

void main()
{
    vec4 texel = texture(sampler, frag_tex_coord);
    
    if(texturing_mode == 1)
    {
        /* rgba */
        gl_FragColor = vec4(
            color.r * texel.r,
            color.g * texel.g,
            color.b * texel.b,
            color.a * texel.a
        );
    }
    else if(texturing_mode == 2)
    {
        /* Red as alpha. */
        gl_FragColor = vec4(
            color.r,
            color.g,
            color.b,
            color.a * texel.r
        );
    }
    else
    {
        discard;
    }
}