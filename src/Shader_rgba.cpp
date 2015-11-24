#include "Shader_rgba.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

#define R64FX_GET_ATTRIB_LOCATION(name) getAttribLocation(attr_##name, #name)
#define R64FX_GET_UNIFORM_LOCATION(name) getUniformLocation(unif_##name, #name)

const char* rgba_vert_text =
#include "rgba.vert.h"
;

const char* rgba_frag_text =
#include "rgba.frag.h"
;


Shader_rgba::Shader_rgba()
:ShadingProgram(rgba_vert_text, rgba_frag_text)
{
    if(!isOk())
        return;

    R64FX_GET_ATTRIB_LOCATION(position);
    R64FX_GET_ATTRIB_LOCATION(color);
    R64FX_GET_UNIFORM_LOCATION(sxsytxty);
}


Shader_rgba::~Shader_rgba()
{

}


void Shader_rgba::setScaleAndShift(float sx, float sy, float tx, float ty)
{
    gl::Uniform4f(unif_sxsytxty, sx, sy, tx, ty);
}


VertexArray_rgba::VertexArray_rgba(Shader_rgba* shader, int size) : VertexArray(size)
{
    gl::GenBuffers(1, &m_position_vbo);
    gl::GenBuffers(1, &m_color_vbo);

    gl::BindVertexArray(vao());
    gl::EnableVertexAttribArray(shader->attr_position);
    gl::EnableVertexAttribArray(shader->attr_color);

    gl::BindBuffer(GL_ARRAY_BUFFER, m_position_vbo);
    gl::BufferData(GL_ARRAY_BUFFER, size*2*sizeof(float), nullptr, GL_STREAM_DRAW);
    gl::VertexAttribPointer(shader->attr_position, 2, GL_FLOAT, GL_FALSE, 0, 0);

    gl::BindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
    gl::BufferData(GL_ARRAY_BUFFER, size*4*sizeof(unsigned char), nullptr, GL_STREAM_DRAW);
    gl::VertexAttribPointer(shader->attr_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
}


VertexArray_rgba::~VertexArray_rgba()
{
    gl::DeleteBuffers(1, &m_position_vbo);
    gl::DeleteBuffers(1, &m_color_vbo);
}


void VertexArray_rgba::loadPositions(float* points, int offset, int count)
{
    int s = sizeof(float)*2;
    gl::BindBuffer(GL_ARRAY_BUFFER, m_position_vbo);
    gl::BufferSubData(GL_ARRAY_BUFFER, offset*s, count*s, points);
}


void VertexArray_rgba::loadColors(unsigned char* colors, int offset, int count)
{
    int s = sizeof(unsigned char)*4;
    gl::BindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
    gl::BufferSubData(GL_ARRAY_BUFFER, offset*s, count*s, colors);
}

}//namespace r64fx