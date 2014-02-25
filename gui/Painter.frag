#version 130

in vec2 frag_tex_coord;

uniform vec4 color;

// uniform sampler2D sampler;


void main()
{
//     vec4 texel = texture2D(sampler, frag_tex_coord);
    
    gl_FragColor = vec4(
        1.0,
        0.0,
        0.0,
        color.a
    );
}