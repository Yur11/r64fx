#version 130

in vec2 frag_tex_coord;

uniform int        mode     = 0;
uniform vec4       color    = vec4(0.0, 0.0, 0.0, 0.0);
uniform sampler2D  sampler;

void main()
{
    vec4 texel = texture(sampler, frag_tex_coord);
    switch(mode)
    {
        //putImage()
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

        //blendColors()
        case 5:
        {
            gl_FragColor = vec4(color[0], color[1], color[2], 1.0 - texel[0]);
            break;
        }

        case 6:
        {
            gl_FragColor = vec4(color[0], color[1], color[2], 1.0 - texel[1]);
            break;
        }

        case 7:
        {
            gl_FragColor = vec4(color[0], color[1], color[2], 1.0 - texel[2]);
            break;
        }

        case 8:
        {
            gl_FragColor = vec4(color[0], color[1], color[2], 1.0 - texel[3]);
            break;
        }

        //fillRect();
        case 9:
        {
            gl_FragColor = color;
            break;
        }

        default:
            discard;
    }
}
