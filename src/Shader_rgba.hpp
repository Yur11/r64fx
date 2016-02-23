#ifdef R64FX_USE_GL

#ifndef R64FX_SHADER_RGBA_HPP
#define R64FX_SHADER_RGBA_HPP

#include "Shader.hpp"

namespace r64fx{

class VertexArray_rgba;

/** @brief ShadingProgram with single color per vertex. */
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

}//namespace r64fx

#endif//R64FX_SHADER_RGBA_HPP

#endif//R64FX_USE_GL