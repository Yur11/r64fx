#ifdef R64FX_USE_GL

#ifndef R64FX_SHADER_RGBA_HPP
#define R64FX_SHADER_RGBA_HPP

#include "Shader.hpp"

namespace r64fx{

class VertexArray_rgba;

/** @brief ShadingProgram with single color per vertex. */
class Shader_rgba : public ShadingProgram{
    GLint attr_position;
    GLint unif_color;
    GLint unif_sxsytxty;

public:
    Shader_rgba();

    virtual ~Shader_rgba();

    void setScaleAndShift(float sx, float sy, float tx, float ty);
    
    void setColor(float r, float g, float b, float a);
    
    void bindPositionAttr(GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer);    
};

}//namespace r64fx

#endif//R64FX_SHADER_RGBA_HPP

#endif//R64FX_USE_GL
