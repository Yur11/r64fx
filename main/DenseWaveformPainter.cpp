#include "DenseWaveformPainter.hpp"
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
    
    position_attr = gl::GetAttribLocation(sp.id(), "position");
    if(position_attr == -1)
    {
        cerr << "Failed to get position_attr location!\n";
        return false;
    }
    
    ref_color_attr = gl::GetAttribLocation(sp.id(), "ref_color");
    if(ref_color_attr == -1)
    {
        cerr << "Failed to get ref_color_attr location!\n";
        return false;
    }
    
    tex_coord_attr = gl::GetAttribLocation(sp.id(), "tex_coord");
    if(tex_coord_attr == -1)
    {
        cerr << "Failed to get tex_coord location!\n";
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
    
#ifdef DEBUG
    const int data_size = 256;
    float data[data_size];
    for(int i=0; i<data_size; i++)
    {
        data[i] = float(i)/float(data_size);
    }
    
    gl::GenTextures(1, &debug_tex);
    gl::BindTexture(GL_TEXTURE_1D, debug_tex);
    gl::TexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl::TexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
//     gl::TexImage1D(GL_TEXTURE_1D, 0, GL_R32F, data_size, 0, GL_RED, GL_FLOAT, data);
    gl::TexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, data_size);
    gl::TexSubImage1D(GL_TEXTURE_1D, 0, 0, data_size, GL_RED, GL_FLOAT, data);
    
    gl::GenerateMipmap(GL_TEXTURE_1D);
    gl::BindTexture(GL_TEXTURE_1D, 0);
#endif//DEBUG
    
    return true;
}


void DenseWaveformPainter::setTexture(GLuint tex)
{
    gl::ActiveTexture(GL_TEXTURE0);
    
    gl::BindTexture(GL_TEXTURE_1D, tex);
    
    gl::Uniform1i(sampler_uniform, 0);
}
    
    
void DenseWaveformPainter::setupAttributes(int nbytes)
{
    int size = nbytes / 4;
    
    gl::EnableVertexAttribArray(position_attr);
    gl::VertexAttribPointer(position_attr, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    gl::EnableVertexAttribArray(ref_color_attr);
    gl::VertexAttribPointer(ref_color_attr, 1, GL_FLOAT, GL_FALSE, 0, size * 0.5 * sizeof(float));
    
    gl::EnableVertexAttribArray(tex_coord_attr);
    gl::VertexAttribPointer(tex_coord_attr, 1, GL_FLOAT, GL_FALSE, 0, size * 0.75 * sizeof(float));
}


void DenseWaveformPainter::setData(float *ptr)
{
    gl::BufferSubData(
        GL_ARRAY_BUFFER,
        0,
        nbytes(),
        ptr
    );
}


void DenseWaveformPainter::setPositions(float* ptr, int nfloats, int offset)
{
    gl::BufferSubData(
        GL_ARRAY_BUFFER, 
        offset * sizeof(float),
        nfloats * sizeof(float), 
        ptr
    );
}


void DenseWaveformPainter::render(GLenum mode, int nvertices, int offset)
{
    gl::DrawArrays(mode, offset, nvertices);
}
   
}//namespace r64fx

