#version 130

in vec2 frag_tex_coord;

uniform sampler2D sampler2d;

void main()
{
    gl_FragColor = texture(sampler2d, frag_tex_coord);
}
