#include "Painter.h"
#include <iostream>

using namespace std;

namespace r64fx{

ShadingProgram   Painter::sp;

GLint            Painter::position_attr;
GLint            Painter::tex_coord_attr;

GLint            Painter::sxsytxty_uniform;
GLint            Painter::color_uniform;
GLint            Painter::sampler_uniform;
GLint            Painter::texturing_mode_uniform;

GLuint           Painter::plain_tex;

int              Painter::current_texturing_mode = Painter::RGBA;

bool Painter::init()
{
    sp = ShadingProgram::create(
        #include "Painter.vert.h"
        ,
        #include "Painter.frag.h"
    );
    
    if(!sp.isOk())
    {
        cerr << "Problems with shading program!\n";
        cerr << sp.infoLog();
        return false;
    }
    
    position_attr = gl::GetAttribLocation(sp.id(), "position");
    if(position_attr == -1)
    {
        cerr << "Failed to get position_attr location!\n";
        return false;
    }
    
    tex_coord_attr = gl::GetAttribLocation(sp.id(), "tex_coord");
    if(tex_coord_attr == -1)
    {
        cerr << "Failed to get tex_coord_attr location!\n";
        return false;
    }
    
    sxsytxty_uniform = gl::GetUniformLocation(sp.id(), "sxsytxty");\
    if(sxsytxty_uniform == -1)
    {
        cerr << "Failed to get sxsytxty_uniform location!\n";
        return false;
    }
    
    sampler_uniform = gl::GetUniformLocation(sp.id(), "sampler");
    if(sampler_uniform == -1)
    {
        cerr << "Failed to get sampler_uniform location!\n";
        return false;
    }
    
    color_uniform = gl::GetUniformLocation(sp.id(), "color");
    if(color_uniform == -1)
    {
        cerr << "Failed to get color_uniform location!\n";
        return false;
    }
    
    texturing_mode_uniform = gl::GetUniformLocation(sp.id(), "texturing_mode");
    if(texturing_mode_uniform == -1)
    {
        cerr << "Failed to get texturing_mode_uniform location!\n";
        return false;
    }
    
    unsigned char plain_tex_data[16 * 16 * 4];
    for(int i=0; i<16*16*4; i++)
        plain_tex_data[i] = 255;
        
    gl::ActiveTexture(GL_TEXTURE0);
    gl::GenTextures(1, &plain_tex);
    gl::BindTexture(GL_TEXTURE_2D, plain_tex);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl::TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, plain_tex_data);
    gl::GenerateMipmap(GL_TEXTURE_2D);
    gl::BindTexture(GL_TEXTURE_2D, 0);
    
    return true;
}


void Painter::setTexture(GLuint tex)
{
    gl::ActiveTexture(GL_TEXTURE0);
    
    gl::BindTexture(GL_TEXTURE_2D, tex);
    
    gl::Uniform1i(sampler_uniform, 0);
}
    
    
void Painter::setupAttributes(int nbytes)
{
    gl::EnableVertexAttribArray(Painter::position_attr);
    gl::VertexAttribPointer(Painter::position_attr, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    gl::EnableVertexAttribArray(Painter::tex_coord_attr);
    gl::VertexAttribPointer(Painter::tex_coord_attr, 2, GL_FLOAT, GL_FALSE, 0, nbytes >> 1);
}


void Painter::setData(float *ptr)
{
    gl::BufferSubData(
        GL_ARRAY_BUFFER,
        0,
        nbytes(),
        ptr
    );
}


void Painter::setPositions(float* ptr, int nfloats, int offset)
{
    gl::BufferSubData(
        GL_ARRAY_BUFFER, 
        offset * sizeof(float),
        nfloats * sizeof(float), 
        ptr
    );
}
    
    
void Painter::setTexCoords(float* ptr, int nfloats, int offset)
{
    gl::BufferSubData(
        GL_ARRAY_BUFFER, 
        nbytes() / 2 + offset * sizeof(float), 
        nfloats * sizeof(float), 
        ptr
    );
}


void Painter::render(GLenum mode, int nvertices, int offset)
{
    gl::DrawArrays(mode, offset, nvertices);
}
   
}//namespace r64fx

