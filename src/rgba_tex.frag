#version 130

in vec2 frag_tex_coord;

uniform sampler2D sampler;

void main()
{
    gl_FragColor = texture(sampler, frag_tex_coord);
}