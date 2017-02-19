#version 130

in vec2 frag_tex_coord;

uniform vec4 color;

uniform sampler2D sampler;

uniform int texture_component;

void main()
{
    vec4 texel = texture(sampler, frag_tex_coord);

    gl_FragColor = vec4(
        color[0],
        color[1],
        color[2],
        1.0 - texel[texture_component]
    );
}
