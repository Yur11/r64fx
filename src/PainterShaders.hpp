#ifdef R64FX_USE_GL

#ifndef R64FX_PAINTER_SHADERS_HPP
#define R64FX_PAINTER_SHADERS_HPP

#include "Shader.hpp"

#ifndef R64FX_PAINTER_SHADER_IMPL
#define R64FX_PAINTER_SHADER_EXTERN extern
#define R64FX_PAINTER_SHADER_NULLPTR
#else
#define R64FX_PAINTER_SHADER_EXTERN
#define R64FX_PAINTER_SHADER_NULLPTR = nullptr
#endif

#define R64FX_DEF_PAINTER_SHADER(NAME) R64FX_PAINTER_SHADER_EXTERN NAME* g_##NAME R64FX_PAINTER_SHADER_NULLPTR;


namespace r64fx{

class PainterShader : public ShadingProgram{
    GLint attr_position;
    GLint unif_sxsytxty;

public:
    PainterShader(VertexShader vert, FragmentShader frag);

    ~PainterShader();

    inline void bindPositionAttr(GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer)
    {
        bindAttribute(attr_position, 2, type, normalized, stride, pointer);
    }

    inline void setScaleAndShift(float sx, float sy, float tx, float ty)
    {
        gl::Uniform4f(unif_sxsytxty, sx, sy, tx, ty);
    }
};


class PainterShader_Common : public PainterShader{
    GLint attr_tex_coord;
    GLint unif_mode;
    GLint unif_color;
    GLint unif_sampler;

public:
    PainterShader_Common();

    inline void bindTexCoordAttr(GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer)
    {
        bindAttribute(attr_tex_coord, 2, type, normalized, stride, pointer);
    }

    inline void setMode(int mode)
    {
        gl::Uniform1i(unif_mode, mode);
    }

    inline int static ModePutImage(int component_count)
    {
        return component_count;
    }

    inline int static ModeBlendColors(int tex_component)
    {
        return 4 | tex_component;
    }

    inline int static ModeColor()
    {
        return 9;
    }

    inline void setColor(float r, float g, float b, float a)
    {
        gl::Uniform4f(unif_color, r, g, b, a);
    }

    inline void setSampler(int sampler)
    {
        gl::Uniform1i(unif_sampler, sampler);
    }
};

R64FX_DEF_PAINTER_SHADER(PainterShader_Common)


void init_painter_shaders();

void cleanup_painter_shaders();

}//namespace r64fx

#undef R64FX_PAINTER_SHADER_EXTERN
#undef R64FX_DEF_PAINTER_SHADER

#endif//R64FX_PAINTER_SHADERS_HPP

#endif//R64FX_USE_GL

