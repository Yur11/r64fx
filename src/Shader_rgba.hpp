#ifndef R64FX_SHADER_RGBA_HPP
#define R64FX_SHADER_RGBA_HPP

#include "Shader.hpp"
#include "VertexArray.hpp"

namespace r64fx{

class Shader_rgba : public ShadingProgram{
    friend class VertexArray_rgba;
    GLint attr_position;
    GLint attr_color;
    GLint unif_sxsytxty;

public:
    Shader_rgba();

    virtual ~Shader_rgba();

    void setScaleAndShift(float sx, float sy, float tx, float ty);
};


class VertexArray_rgba : public VertexArray{
    GLuint m_position_vbo;
    GLuint m_color_vbo;

public:
    VertexArray_rgba(Shader_rgba* shader, int size);

    virtual ~VertexArray_rgba();

    void loadPositions(float* points, int offset, int count);

    void loadColors(unsigned char* colors, int offset, int count);
};


}//namespace r64fx

#endif//R64FX_SHADER_RGBA_HPP