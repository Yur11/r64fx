#version 130

in vec2 frag_tex_coord;

uniform vec4 color;
uniform sampler2D sampler;
uniform int texturing_mode = 1;

void main()
{
    vec4 texel = texture(sampler, frag_tex_coord);
    
    switch(texturing_mode)
    {
        case 1:
        {
            /* rgba */
            gl_FragColor = vec4(
                color.r * texel.r,
                color.g * texel.g,
                color.b * texel.b,
                color.a * texel.a
            );
            break;
        }
        
        case 2:
        {
            /* Red as alpha. */
            gl_FragColor = vec4(
                color.r,
                color.g,
                color.b,
                color.a * texel.r
            );
            break;
        }
        
        default:
        {
            discard;
        }
    }
}