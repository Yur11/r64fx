#ifndef R64FX_SHADER_RGBA_TEX_HPP
#define R64FX_SHADER_RGBA_TEX_HPP

#include "Shader.hpp"

namespace r64fx{

/** @brief ShadingProgram with a rgba texture. */
class Shader_rgba_tex : public ShadingProgram{
    friend class VertexArray_rgba;
    GLint attr_position;
    GLint attr_tex_coord;
    GLint unif_sxsytxty;
    GLint unif_sampler;

public:
    Shader_rgba_tex();

    virtual ~Shader_rgba_tex();

    void setScaleAndShift(float sx, float sy, float tx, float ty);

    void setSampler(int sampler);
};

}//namespace r64fx

#endif//R64FX_SHADER_RGBA_TEX_HPP