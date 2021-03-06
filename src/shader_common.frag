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
uniform int        zero_index    = 0;
uniform int        zero_count    = 0;
uniform int        pole_index    = 0;
uniform int        pole_count    = 0;

const float PI = 3.1415926;
const float PI_rcp = 1.0 / PI;
const float rcp100 = 1.0 / 100.0;

vec2 complex_add(in vec2 a, in vec2 b)
{
    return vec2(a.x + b.x, a.y + b.y);
}

vec2 complex_sub(in vec2 a, in vec2 b)
{
    return vec2(a.x - b.x, a.y - b.y);
}

vec2 complex_mul(in vec2 a, in vec2 b)
{
    return vec2(a.x*b.x - a.y*b.y, a.y*b.x + a.x*b.y);
}

vec2 complex_div(in vec2 a, in vec2 b)
{
    return vec2(
        (a.x * b.x + a.y * b.y) / (b.x * b.x + b.y * b.y),
        (a.y * b.x - a.x * b.y) / (b.x * b.x + b.y * b.y)
    );
}

float complex_maginitude(in vec2 c)
{
    return sqrt(c[0] * c[0] + c[1] * c[1]);
}

float complex_phase(in vec2 c)
{
    return atan(c[1], c[0]);
}


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

        case 12:
        {
            vec2 z = frag_tex_coord;

            vec2 numerator = vec2(1.0, 0.0);
            for(int i=0; i<zero_count; i++)
            {
                vec2 zero = texelFetch(sampler1d, i + zero_index, 0).xy;
                numerator = complex_mul(numerator, complex_sub(z, zero));
            }

            vec2 denominator = vec2(1.0, 0.0);
            for(int i=0; i<pole_count; i++)
            {
                vec2 pole = texelFetch(sampler1d, i + pole_index, 0).xy;
                denominator = complex_mul(denominator, complex_sub(z, pole));
            }

            float mag = complex_maginitude(denominator);
            if(mag > 0.0)
            {
                vec2 h = complex_div(numerator, denominator);
                mag = complex_maginitude(h);
                mag = (mag < 100 ? mag * rcp100 : 1.0);

                float phase = complex_phase(h);
                float r = 0.0;
                float b = 0.0;

                if(phase > 0.0)
                {
                    while(phase > PI)
                        phase -= PI;
                    r = abs(0.5 - phase * PI_rcp) * 2.0;
                }

                if(phase < 0.0)
                {
                    while(phase < -PI)
                        phase += PI;
                    b = abs(0.5 - abs(phase) * PI_rcp) * 2.0;
                }

                r *= mag;
                b *= mag;

                gl_FragColor = vec4(mag - r, mag - r - b, mag - b, 0.0);
                break;
            }
            gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        }
        break;

        default:
            discard;
    }
}
