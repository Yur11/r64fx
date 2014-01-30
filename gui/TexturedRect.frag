#version 130

uniform sampler2D sampler;

in vec2 fragment_tex_coord;

void main()
{
    vec4 texel = texture(sampler, fragment_tex_coord);
    
    gl_FragColor = vec4(
        texel.r, 
        texel.g,
        texel.b, 
        texel.a
    );
    
    
}