#include "DenseWaveformPainter.h"
#include <iostream>

using namespace std;

namespace r64fx{

ShadingProgram   DenseWaveformPainter::sp;

GLint            DenseWaveformPainter::position_attr;
GLint            DenseWaveformPainter::ref_color_attr;
GLint            DenseWaveformPainter::tex_coord_attr;

GLint            DenseWaveformPainter::sxsytxty_uniform; //Projection2D
GLint            DenseWaveformPainter::sampler_uniform;

#ifdef DEBUG
GLuint           DenseWaveformPainter::debug_tex;
#endif//DEBUG

bool DenseWaveformPainter::init()
{
    sp = ShadingProgram::create(
        #include "DenseWaveformPainter.vert.h"
        ,
        #include "DenseWaveformPainter.frag.h"
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
    
    ref_color_attr = glGetAttribLocation(sp.id(), "ref_color");
    CHECK_FOR_GL_ERRORS;
    if(ref_color_attr == -1)
    {
        cerr << "Failed to get ref_color_attr location!\n";
        return false;
    }
    
    tex_coord_attr = glGetAttribLocation(sp.id(), "tex_coord");
    CHECK_FOR_GL_ERRORS;
    if(tex_coord_attr == -1)
    {
        cerr << "Failed to get tex_coord location!\n";
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
    
#ifdef DEBUG
    const int data_size = 256;
    float data[data_size];
    for(int i=0; i<data_size; i++)
    {
        data[i] = float(i)/float(data_size);
    }
    
    glGenTextures(1, &debug_tex);                                                 CHECK_FOR_GL_ERRORS;
    glBindTexture(GL_TEXTURE_1D, debug_tex);                                      CHECK_FOR_GL_ERRORS;
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);             CHECK_FOR_GL_ERRORS;
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);             CHECK_FOR_GL_ERRORS;
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RED, data_size, 0, GL_RED, GL_FLOAT, data); CHECK_FOR_GL_ERRORS;
    glGenerateMipmap(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, 0);                                              CHECK_FOR_GL_ERRORS;
#endif//DEBUG
    
    return true;
}


void DenseWaveformPainter::setTexture(GLuint tex)
{
    glActiveTexture(GL_TEXTURE0);
    CHECK_FOR_GL_ERRORS;
    
    glBindTexture(GL_TEXTURE_1D, tex);
    CHECK_FOR_GL_ERRORS;
    
    glUniform1i(sampler_uniform, 0);
    CHECK_FOR_GL_ERRORS;
}
    
    
void DenseWaveformPainter::setupAttributes(int nbytes)
{
    int size = nbytes / 4;
    
    glEnableVertexAttribArray(position_attr);
    CHECK_FOR_GL_ERRORS;
    glVertexAttribPointer(position_attr, 2, GL_FLOAT, GL_FALSE, 0, 0);
    CHECK_FOR_GL_ERRORS;
    
    glEnableVertexAttribArray(ref_color_attr);
    CHECK_FOR_GL_ERRORS;
    glVertexAttribPointer(ref_color_attr, 1, GL_FLOAT, GL_FALSE, 0, (void*)(long)(size * 0.5 * sizeof(float)));
    CHECK_FOR_GL_ERRORS;
    
    glEnableVertexAttribArray(tex_coord_attr);
    CHECK_FOR_GL_ERRORS;
    glVertexAttribPointer(tex_coord_attr, 1, GL_FLOAT, GL_FALSE, 0, (void*)(long)(size * 0.75 * sizeof(float)));
    CHECK_FOR_GL_ERRORS;
}


void DenseWaveformPainter::setData(float *ptr)
{
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        nbytes(),
        ptr
    );
    CHECK_FOR_GL_ERRORS;
}


void DenseWaveformPainter::setPositions(float* ptr, int nfloats, int offset)
{
    glBufferSubData(
        GL_ARRAY_BUFFER, 
        offset * sizeof(float),
        nfloats * sizeof(float), 
        ptr
    );
    CHECK_FOR_GL_ERRORS;
}


void DenseWaveformPainter::render(GLenum mode, int nvertices, int offset)
{
    glDrawArrays(mode, offset, nvertices);
    CHECK_FOR_GL_ERRORS;
}
   
}//namespace r64fx

