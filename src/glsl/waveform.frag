#version 130

in vec2 frag_tex_coord;

uniform vec4 color;

uniform sampler1D sampler;

void main()
{
    vec4 texel = texture(sampler, frag_tex_coord[0]);
    if(frag_tex_coord[1] > texel[0] && frag_tex_coord[1] < texel[1])
    {
        gl_FragColor = color;
    }
    else
    {
        discard;
    }
}
