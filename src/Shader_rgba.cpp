#ifdef R64FX_USE_GL

#include "Shader_rgba.hpp"
#include "ShaderImplUtils.hpp"

namespace r64fx{

namespace{
const char* vert_text =
#include "rgba.vert.h"
;

const char* frag_text =
#include "rgba.frag.h"
;
}


Shader_rgba::Shader_rgba()
:ShadingProgram(vert_text, frag_text)
{
    if(!isOk())
        return;

    R64FX_GET_ATTRIB_LOCATION(position);
    R64FX_GET_UNIFORM_LOCATION(color);
    R64FX_GET_UNIFORM_LOCATION(sxsytxty);
}


Shader_rgba::~Shader_rgba()
{

}


void Shader_rgba::setScaleAndShift(float sx, float sy, float tx, float ty)
{
    gl::Uniform4f(unif_sxsytxty, sx, sy, tx, ty);
}


void Shader_rgba::setColor(float r, float g, float b, float a)
{
    gl::Uniform4f(unif_color, r, g, b, a);
}


void Shader_rgba::bindPositionAttr(GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer)
{
    bindAttribute(attr_position, 2, type, normalized, stride, pointer);
}


}//namespace r64fx

#endif//R64FX_USE_GL
