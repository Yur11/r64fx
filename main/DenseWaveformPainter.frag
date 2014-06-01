#version 130

in float frag_ref_color;

in float frag_tex_coord;

uniform sampler1D sampler;


void main()
{
    if(frag_tex_coord == 0.0)
    {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else 
    {
        vec4 tex = texture(sampler, frag_tex_coord);
        float ref_color = (frag_ref_color - 0.5) * 2;
        
        if(ref_color > 0)
        {
            if(tex.r > ref_color)
                gl_FragColor = vec4(abs(ref_color), 0.3, 0.3, 1.0);
            else
                discard;
        }
        else
        {
            if(tex.g < ref_color)
                gl_FragColor = vec4(abs(ref_color), 0.3, 0.3, 1.0);
            else
                discard; 
        }
    }
}