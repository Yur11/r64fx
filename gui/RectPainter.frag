#version 130

uniform sampler2D sampler;

uniform vec4 color;

in vec2 fragment_tex_coord;

void main()
{
    vec4 texel = texture(sampler, fragment_tex_coord);
    
    gl_FragColor = vec4(
        texel.r * color.r,
        texel.g * color.g,
        texel.b * color.b, 
        texel.a * color.a
    );
    
    
}