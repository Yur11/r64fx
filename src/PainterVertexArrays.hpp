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

    /* x,y clockwise */
    void setCoords(short* buff, int nvertices);

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

    void setTexCoords(short left, short top, short right, short bottom, bool flip_vert = false, bool flip_hori = false, bool flip_diag = false);
};

}//namespace r64x

#endif//PAINTER_VERTEX_ARRAYS_HPP

#endif//R64FX_USE_GL
