#version 130

in vec2 frag_tex_coord;

uniform sampler2D sampler;
uniform int component_count = 0;

void main()
{
    vec4 texel = texture(sampler, frag_tex_coord);
    switch(component_count)
    {
        case 1:
        {
            gl_FragColor = vec4(texel[0], texel[0], texel[0], 0.0);
            break;
        }

        case 2:
        {
            gl_FragColor = vec4(texel[0], texel[0], texel[0], texel[1]);
            break;
        }

        case 3:
        {
            gl_FragColor = vec4(texel[0], texel[1], texel[2], 0.0);
            break;
        }

        case 4:
        {
            gl_FragColor = texel;
            break;
        }

        default:
            discard;
    }
}
