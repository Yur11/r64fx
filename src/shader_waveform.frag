#version 130

in vec2 frag_tex_coord;

uniform vec4 color;

uniform sampler1D sampler;

uniform float gain = 1.0;

void main()
{
    vec4 texel = texture(sampler, frag_tex_coord[0]);
    float min_value = texel[0] * gain;
    float max_value = texel[1] * gain; 
    if(frag_tex_coord[1] > min_value && frag_tex_coord[1] < max_value)
    {
        gl_FragColor = color;
    }
    else
    {
        discard;
    }
}
