#version 130

in vec2 frag_tex_coord;

uniform vec4 color = vec4(1.0, 1.0, 1.0, 0.0);

uniform sampler2D sampler;

void main()
{
    vec4 texel = texture(sampler, frag_tex_coord);
    
    gl_FragColor = vec4(
        color[0] * texel[0],
        color[1] * texel[0],
        color[2] * texel[0],
        0.0
    );
}
