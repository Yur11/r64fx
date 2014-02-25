#version 130

uniform sampler1D sampler;

uniform vec4 color;

in float tex_coord;

void main()
{
   vec4 texel = texture(sampler, tex_coord);
   
   gl_FragColor = vec4(
       color.r * texel.r, 
       color.g * texel.r, 
       color.b * texel.r,
       color.a
   );
}