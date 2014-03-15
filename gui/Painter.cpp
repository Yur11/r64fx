#include "Painter.h"
#include <iostream>

using namespace std;

namespace r64fx{

ShadingProgram   Painter::sp;

GLint            Painter::position_attr;
GLint            Painter::tex_coord_attr;

GLint            Painter::sxsytxty_uniform; //Projection2D
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
    
    position_attr = glGetAttribLocation(sp.id(), "position");
    CHECK_FOR_GL_ERRORS; 
    if(position_attr == -1)
    {
        cerr << "Failed to get position_attr location!\n";
        return false;
    }
    
    tex_coord_attr = glGetAttribLocation(sp.id(), "tex_coord");
    CHECK_FOR_GL_ERRORS; 
    if(tex_coord_attr == -1)
    {
        cerr << "Failed to get tex_coord_attr location!\n";
        return false;
    }
    
    sxsytxty_uniform = glGetUniformLocation(sp.id(), "sxsytxty");\
    CHECK_FOR_GL_ERRORS; 
    if(sxsytxty_uniform == -1)
    {
        cerr << "Failed to get sxsytxty_uniform location!\n";
        return false;
    }
    
    sampler_uniform = glGetUniformLocation(sp.id(), "sampler");
    CHECK_FOR_GL_ERRORS; 
    if(sampler_uniform == -1)
    {
        cerr << "Failed to get sampler_uniform location!\n";
        return false;
    }
    
    color_uniform = glGetUniformLocation(sp.id(), "color");
    CHECK_FOR_GL_ERRORS; 
    if(color_uniform == -1)
    {
        cerr << "Failed to get color_uniform location!\n";
        return false;
    }
    
    texturing_mode_uniform = glGetUniformLocation(sp.id(), "texturing_mode");
    CHECK_FOR_GL_ERRORS;
    if(texturing_mode_uniform == -1)
    {
        cerr << "Failed to get texturing_mode_uniform location!\n";
        return false;
    }
    
    unsigned char plain_tex_data[16 * 16 * 4];
    for(int i=0; i<16*16*4; i++)
        plain_tex_data[i] = 255;
        
    glActiveTexture(GL_TEXTURE0);
    CHECK_FOR_GL_ERRORS;
    
    glGenTextures(1, &plain_tex);                                          CHECK_FOR_GL_ERRORS; 
    glBindTexture(GL_TEXTURE_2D, plain_tex);                               CHECK_FOR_GL_ERRORS; 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);     CHECK_FOR_GL_ERRORS; 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);     CHECK_FOR_GL_ERRORS;
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);           CHECK_FOR_GL_ERRORS;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, plain_tex_data);
    CHECK_FOR_GL_ERRORS;
    
    glBindTexture(GL_TEXTURE_2D, 0);         CHECK_FOR_GL_ERRORS; 
    
    return true;
}


void Painter::setTexture(GLuint tex)
{
    glActiveTexture(GL_TEXTURE0);
    CHECK_FOR_GL_ERRORS;
    
    glBindTexture(GL_TEXTURE_2D, tex);
    CHECK_FOR_GL_ERRORS;
    
    glUniform1i(sampler_uniform, 0);
    CHECK_FOR_GL_ERRORS;
}


// Painter::Vertex::Vertex(float x, float y, float s, float t)
// : vec{ x, y, s, t }
// {
//     
// }


// void Painter::Vertex::setupAttributes(int nbytes)
// {
//     glEnableVertexAttribArray(position_attr);
//     CHECK_FOR_GL_ERRORS; 
//     glVertexAttribPointer(position_attr, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, 0);
//     CHECK_FOR_GL_ERRORS; 
//     
//     glEnableVertexAttribArray(tex_coord_attr);
//     CHECK_FOR_GL_ERRORS; 
//     glVertexAttribPointer(tex_coord_attr, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)(sizeof(float)*2));
//     CHECK_FOR_GL_ERRORS; 
// }
    
    
void Painter::setupAttributes(int nbytes)
{
    glEnableVertexAttribArray(Painter::position_attr);
    CHECK_FOR_GL_ERRORS;
    glVertexAttribPointer(Painter::position_attr, 2, GL_FLOAT, GL_FALSE, 0, 0);
    CHECK_FOR_GL_ERRORS;
    
    glEnableVertexAttribArray(Painter::tex_coord_attr);
    CHECK_FOR_GL_ERRORS;
    glVertexAttribPointer(Painter::tex_coord_attr, 2, GL_FLOAT, GL_FALSE, 0, (void*)(long)(nbytes >> 1));
}


void Painter::setData(float *ptr)
{
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        nbytes(),
        ptr
    );
    CHECK_FOR_GL_ERRORS;
}


void Painter::setPositions(float* ptr, int nfloats, int offset)
{
    glBufferSubData(
        GL_ARRAY_BUFFER, 
        offset * sizeof(float),
        nfloats * sizeof(float), 
        ptr
    );
    CHECK_FOR_GL_ERRORS;
}
    
    
void Painter::setTexCoords(float* ptr, int nfloats, int offset)
{
    glBufferSubData(
        GL_ARRAY_BUFFER, 
        nbytes() / 2 + offset * sizeof(float), 
        nfloats * sizeof(float), 
        ptr
    );
    CHECK_FOR_GL_ERRORS;
}


void Painter::render(GLenum mode, int nvertices, int offset)
{
    glDrawArrays(mode, offset, nvertices);
    CHECK_FOR_GL_ERRORS;
}
   
}//namespace r64fx

