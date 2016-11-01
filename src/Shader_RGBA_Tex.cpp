#ifdef R64FX_USE_GL

#include "Shader_RGBA_Tex.hpp"
#include "ShaderImplUtils.hpp"

namespace r64fx{

namespace{
const char* vert_text =
#include "rgba_tex.vert.h"
;

const char* frag_text =
#include "rgba_tex.frag.h"
;
}


Shader_rgba_tex::Shader_rgba_tex()
:ShadingProgram(vert_text, frag_text)
{
    if(!isOk())
        return;

    R64FX_GET_ATTRIB_LOCATION(position);
    R64FX_GET_ATTRIB_LOCATION(tex_coord);
    R64FX_GET_UNIFORM_LOCATION(sxsytxty);
    R64FX_GET_UNIFORM_LOCATION(sampler);
}


Shader_rgba_tex::~Shader_rgba_tex()
{

}


void Shader_rgba_tex::setScaleAndShift(float sx, float sy, float tx, float ty)
{
    gl::Uniform4f(unif_sxsytxty, sx, sy, tx, ty);
}


void Shader_rgba_tex::setSampler(int sampler)
{
    gl::Uniform1i(unif_sampler, sampler);
}


void Shader_rgba_tex::bindPositionAttr(GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer)
{
    bindAttribute(attr_position, 2, type, normalized, stride, pointer);
}


void Shader_rgba_tex::bindTexCoordAttr(GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer)
{
    bindAttribute(attr_tex_coord, 2, type, normalized, stride, pointer);
}

}//namespace r64fx

#endif//R64FX_USE_GL
