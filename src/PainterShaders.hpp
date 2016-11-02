#ifdef R64FX_USE_GL

#ifndef R64FX_PAINTER_SHADERS_HPP
#define R64FX_PAINTER_SHADERS_HPP

#include "Shader.hpp"

namespace r64fx{
    
class ShaderCommon : public ShadingProgram{
    GLint unif_sxsytxty;
    GLint attr_position;
    
public:
    ShaderCommon();
    
    ~ShaderCommon();
    
    void setScaleAndShift(float sx, float sy, float tx, float ty);
    
    void bindPositionAttr(GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer);
    
protected:
    void fetchCommonIndices();
};
    
    
class Shader_Color : public ShaderCommon{
    GLint unif_color;

public:
    Shader_Color();

    ~Shader_Color();
    
    void setColor(float r, float g, float b, float a);
};
    
    
class Shader_Texture : public ShaderCommon{
    GLint unif_sampler;
    GLint attr_tex_coord;

public:
    Shader_Texture();

    ~Shader_Texture();

    void setSampler(int sampler);
    
    void bindTexCoordAttr(GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer);
};
    
}//namespace r64fx

#endif//R64FX_PAINTER_SHADERS_HPP

#endif//R64FX_USE_GL

