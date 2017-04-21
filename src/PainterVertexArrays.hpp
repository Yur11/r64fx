#ifdef R64FX_USE_GL

#ifndef PAINTER_VERTEX_ARRAYS_HPP
#define PAINTER_VERTEX_ARRAYS_HPP

#include "PainterShaders.hpp"

namespace r64fx{

class PainterVertexArray{
protected:
    GLuint m_vao;
    GLuint m_vbo;

public:
    void init(int nbytes);

    void cleanup();

    void setRect(short left, short top, short right, short bottom);

    void draw();
};


class PainterVertexArray_CommonRect : public PainterVertexArray{
public:
    void init()
    {
        PainterVertexArray::init(32);
        g_PainterShader_Common->bindPositionAttr(GL_SHORT, GL_FALSE, 0, 0);
        g_PainterShader_Common->bindTexCoordAttr(GL_SHORT, GL_FALSE, 0, 16);
    }

    void setTexCoords(short left, short top, short right, short bottom)
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
        gl::BufferSubData(GL_ARRAY_BUFFER, 16, 16, buff);
    }
};

}//namespace r64x

#endif//PAINTER_VERTEX_ARRAYS_HPP

#endif//R64FX_USE_GL
