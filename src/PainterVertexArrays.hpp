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

    void setRect(float left, float top, float right, float bottom);

    void draw();
};


class PainterVertexArray_UberRect : public PainterVertexArray{
public:
    void init()
    {
        PainterVertexArray::init(64);
        g_PainterShader_Uber->bindPositionAttr(GL_FLOAT, GL_FALSE, 0, 0);
        g_PainterShader_Uber->bindTexCoordAttr(GL_FLOAT, GL_FALSE, 0, 32);
    }

    void setTexCoords(float left, float top, float right, float bottom)
    {
        float buff[8];

        buff[0] = left;
        buff[1] = top;

        buff[2] = right;
        buff[3] = top;

        buff[4] = right;
        buff[5] = bottom;

        buff[6] = left;
        buff[7] = bottom;

        gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
        gl::BufferSubData(GL_ARRAY_BUFFER, 32, 32, buff);
    }
};

}//namespace r64x

#endif//PAINTER_VERTEX_ARRAYS_HPP

#endif//R64FX_USE_GL
