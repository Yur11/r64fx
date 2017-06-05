#version 130

in vec2 frag_tex_coord;

uniform int        mode          = 0;
uniform vec4       colors[4]     = vec4[4](
    vec4(0.0, 0.0, 0.0, 0.0),
    vec4(0.0, 0.0, 0.0, 0.0),
    vec4(0.0, 0.0, 0.0, 0.0),
    vec4(0.0, 0.0, 0.0, 0.0)
);
uniform sampler1D  sampler1d;
uniform sampler2D  sampler2d;
uniform vec2       rect_size     = vec2(0, 0);
uniform float      stroke_width  = 0;

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
        case 6:
        case 7:
        case 8:
        {
            vec4 frag_color = vec4(0.0, 0.0, 0.0, 0.0);
            vec4 texel = texelFetch(sampler2d, ivec2(frag_tex_coord), 0);
            for(int i=0; i<(mode-4); i++)
            {
                 frag_color += vec4(colors[i][0], colors[i][1], colors[i][2], 1.0 - texel[i]);
            }
            gl_FragColor = frag_color;
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

        case 11:
        {
            ivec2 coord = ivec2(floor(frag_tex_coord[0]), floor(frag_tex_coord[1]));
            bool border = (
                (coord.x < stroke_width) ||
                (coord.y < stroke_width) ||
                (coord.x >= rect_size.x - stroke_width) ||
                (coord.y >= rect_size.y - stroke_width)
            );
            gl_FragColor = colors[border ? 0 : 1];
            break;
        }

        default:
            discard;
    }
}
