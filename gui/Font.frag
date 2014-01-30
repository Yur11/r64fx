#version 130

uniform sampler2D glyph_sampler;

uniform float r, g, b, a;

in vec2 fragment_tex_coord;

void main()
{
    vec4 texel = texture(glyph_sampler, fragment_tex_coord);
    
    gl_FragColor = vec4(
        r, 
        g,
        b, 
        texel.r * a
    );
}