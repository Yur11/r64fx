#include "Painter.h"
#include <iostream>

using namespace std;

namespace r64fx{

ShadingProgram   Painter::sp;
GLint            Painter::xyst_attr;
GLint            Painter::sxsytxty_uniform; //Projection2D
GLint            Painter::color_uniform;
GLint            Painter::sampler_uniform;
GLuint           Painter::sampler;
GLuint           Painter::plain_tex;

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
    
    xyst_attr = glGetAttribLocation(sp.id(), "xyst");
    CHECK_FOR_GL_ERRORS; 
    if(xyst_attr == -1)
    {
        cerr << "Failed to get xyst_attr location!\n";
        return false;
    }
    
    sxsytxty_uniform = glGetUniformLocation(sp.id(), "sxsytxty");\
    CHECK_FOR_GL_ERRORS; 
    if(sxsytxty_uniform == -1)
    {
        cerr << "Failed to get sxsytxty_uniform location!\n";
        return false;
    }
    
    color_uniform = glGetUniformLocation(sp.id(), "color");
    CHECK_FOR_GL_ERRORS; 
    if(color_uniform == -1)
    {
        cerr << "Failed to get color_uniform location!\n";
        return false;
    }
    
//     sampler_uniform = glGetUniformLocation(sp.id(), "sampler");
//     CHECK_FOR_GL_ERRORS; 
//     if(sampler_uniform == -1)
//     {
//         cerr << "Failed to get sampler_uniform location!\n";
//         return false;
//     }
    
    unsigned char plain_tex_data[16 * 16 * 4];
    for(int i=0; i<16*16*4; i++)
        plain_tex_data[i] = 255;
        
    glActiveTexture(GL_TEXTURE0);
    CHECK_FOR_GL_ERRORS;
    
    glGenTextures(1, &plain_tex);                                          CHECK_FOR_GL_ERRORS; 
    glBindTexture(GL_TEXTURE_2D, plain_tex);                               CHECK_FOR_GL_ERRORS; 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);     CHECK_FOR_GL_ERRORS; 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);     CHECK_FOR_GL_ERRORS; 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, plain_tex_data);
    CHECK_FOR_GL_ERRORS;
    
    glBindTexture(GL_TEXTURE_2D, 0);         CHECK_FOR_GL_ERRORS; 
    
    return true;
}


Painter::Vertex::Vertex(float x, float y, float s, float t)
: vec{ x, y, s, t }
{
    
}


void Painter::Vertex::setupAttributes()
{
    glEnableVertexAttribArray(xyst_attr);
    CHECK_FOR_GL_ERRORS; 
    glVertexAttribPointer(xyst_attr, 4, GL_FLOAT, GL_FALSE, 0, 0);
    CHECK_FOR_GL_ERRORS; 
}
    
   
}//namespace r64fx

