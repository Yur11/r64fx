#include "PainterVertexArrays.hpp"
#ifdef R64FX_USE_GL

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

void PainterVertexArray::setRect(short left, short top, short right, short bottom)
{
    short buff[8];

    buff[0] = left;
    buff[1] = top;

    buff[2] = right;
    buff[3] = top;

    buff[4] = right;
    buff[5] = bottom;

    buff[6] = left;
    buff[7] = bottom;

    gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    gl::BufferSubData(GL_ARRAY_BUFFER, 0, 16, buff);
}

void PainterVertexArray::draw()
{
    gl::BindVertexArray(m_vao);
    gl::DrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

}//namespace r64fx

#endif//R64FX_USE_GL
