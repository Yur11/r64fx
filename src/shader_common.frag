#version 130

in vec2 frag_tex_coord;

uniform int        mode        = 0;
uniform vec4       colors[4]    = vec4[4](
    vec4(0.0, 0.0, 0.0, 0.0),
    vec4(0.0, 0.0, 0.0, 0.0),
    vec4(0.0, 0.0, 0.0, 0.0),
    vec4(0.0, 0.0, 0.0, 0.0)
);
uniform sampler1D  sampler1d;
uniform sampler2D  sampler2d;

void main()
{
    switch(mode)
    {
        //putImage()
        case 1:
        {
            vec4 texel = texelFetch(sampler2d, ivec2(frag_tex_coord), 0);
            gl_FragColor = vec4(texel[0], texel[0], texel[0], 0.0);
            break;
        }

        case 2:
        {
            vec4 texel = texelFetch(sampler2d, ivec2(frag_tex_coord), 0);
            gl_FragColor = vec4(texel[0], texel[0], texel[0], texel[1]);
            break;
        }

        case 3:
        {
            vec4 texel = texelFetch(sampler2d, ivec2(frag_tex_coord), 0);
            gl_FragColor = vec4(texel[0], texel[1], texel[2], 0.0);
            break;
        }

        case 4:
        {
            vec4 texel = texelFetch(sampler2d, ivec2(frag_tex_coord), 0);
            gl_FragColor = texel;
            break;
        }

        //blendColors()
        case 5:
        {
            vec4 texel = texelFetch(sampler2d, ivec2(frag_tex_coord), 0);
            gl_FragColor = vec4(colors[0][0], colors[0][1], colors[0][2], 1.0 - texel[0]);
            break;
        }

        case 6:
        {
            vec4 texel = texelFetch(sampler2d, ivec2(frag_tex_coord), 0);
            gl_FragColor = vec4(colors[0][0], colors[0][1], colors[0][2], 1.0 - texel[1]);
            break;
        }

        case 7:
        {
            vec4 texel = texelFetch(sampler2d, ivec2(frag_tex_coord), 0);
            gl_FragColor = vec4(colors[0][0], colors[0][1], colors[0][2], 1.0 - texel[2]);
            break;
        }

        case 8:
        {
            vec4 texel = texelFetch(sampler2d, ivec2(frag_tex_coord), 0);
            gl_FragColor = vec4(colors[0][0], colors[0][1], colors[0][2], 1.0 - texel[3]);
            break;
        }

        //fillRect()
        case 9:
        {
            gl_FragColor = colors[0];
            break;
        }

        //drawWaveform()
        case 10:
        {
            vec4 texel = texelFetch(sampler1d, int(frag_tex_coord[0]), 0);
            float min_value = texel[0];
            float max_value = texel[1];
            float value = float(frag_tex_coord[1]);
            if(value > min_value && value < max_value)
            {
                gl_FragColor = colors[0];
            }
            else
            {
                discard;
            }
            break;
        }

        default:
            discard;
    }
}
