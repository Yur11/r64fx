#version 130

in vec2 frag_tex_coord;

uniform vec4 color;

uniform sampler2D sampler;


void main()
{
    vec4 texel = texture(sampler, frag_tex_coord);
    
    gl_FragColor = vec4(
        color.r * texel.r,
        color.g * texel.g,
        color.b * texel.b,
        color.a * texel.a
    );
}