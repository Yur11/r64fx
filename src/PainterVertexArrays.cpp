#include "PainterVertexArrays.hpp"
#ifdef R64FX_USE_GL

#include <algorithm>

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

using std::swap;

namespace r64fx{

void PainterVertexArray::init(int nbytes)
{
    gl::GenVertexArrays(1, &m_vao);
    gl::BindVertexArray(m_vao);
    gl::GenBuffers(1, &m_vbo);
    gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    gl::BufferData(GL_ARRAY_BUFFER, nbytes, nullptr, GL_STREAM_DRAW);
}

void PainterVertexArray::cleanup()
{
    gl::DeleteVertexArrays(1, &m_vao);
    gl::DeleteBuffers(1, &m_vbo);
}


void PainterVertexArray::setCoords(short* buff, int nvertices)
{
#ifdef R64FX_DEBUG
    assert(buff != nullptr);
    assert(nvertices == 4);
#endif//R64FX_DEBUG
    gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    gl::BufferSubData(GL_ARRAY_BUFFER, 0, (nvertices << 2), buff);
}


void PainterVertexArray::setRect(short left, short top, short right, short bottom)
{
    short buff[8] = {
        left,  top,
        right, top,
        right, bottom,
        left,  bottom
    };
    setCoords(buff, 4);
}

void PainterVertexArray::draw()
{
    gl::BindVertexArray(m_vao);
    gl::DrawArrays(GL_TRIANGLE_FAN, 0, 4);
}


void PainterVertexArray_CommonRect::setTexCoords(short left, short top, short right, short bottom, bool flip_vert, bool flip_hori, bool flip_diag)
{
    short buff[8] = {
        left,  top,
        right, top,
        right, bottom,
        left,  bottom
    };

    if(flip_diag)
        swap(flip_vert, flip_hori);

    if(flip_vert)
    {
        swap(buff[1], buff[7]);
        swap(buff[3], buff[5]);
    }

    if(flip_hori)
    {
        swap(buff[0], buff[2]);
        swap(buff[4], buff[6]);
    }

    if(flip_diag)
    {
        swap(buff[2], buff[6]);
        swap(buff[3], buff[7]);
    }

    gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    gl::BufferSubData(GL_ARRAY_BUFFER, 16, 16, buff);
}

}//namespace r64fx

#endif//R64FX_USE_GL
