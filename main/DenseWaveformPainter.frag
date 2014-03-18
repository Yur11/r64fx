#version 130

in float frag_ref_color;

in float frag_tex_coord;

uniform sampler1D sampler;


void main()
{
    vec4 tex = texture(sampler, frag_tex_coord);
    float ref_color = (frag_ref_color - 0.5) * 2;
    
    if(ref_color > 0)
    {
        if(tex.r > ref_color)
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        else
            discard; 
    }
    else
    {
        if(tex.g < ref_color)
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        else
            discard; 
    }
}