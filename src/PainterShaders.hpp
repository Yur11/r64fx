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

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

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
    GLint unif_colors;
    GLint unif_sampler1d;
    GLint unif_sampler2d;

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

    inline static const int ModePutImage(int component_count)
    {
#ifdef R64FX_DEBUG
        assert(component_count >= 1 && component_count <= 4);
#endif//R64FX_DEBUG
        return component_count;
    }

    inline static const int ModeBlendColors(int component_count)
    {
#ifdef R64FX_DEBUG
        assert(component_count >= 1 &&  component_count <= 4);
#endif//R64FX_DEBUG
        return 4 + component_count;
    }

    inline static const int ModeColor()
    {
        return 9;
    }

    inline static const int ModeWaveform()
    {
        return 10;
    }

    inline void setColors(float* rgba, int location, int count)
    {
        gl::Uniform4fv(unif_colors + location, count, rgba);
    }

    inline void setColor(float r, float g, float b, float a, int location = 0)
    {
        float rgba[4] = {r, g, b, a};
        setColors(rgba, location, 1);
    }

    inline void setSampler1D(int sampler)
    {
        gl::Uniform1i(unif_sampler1d, sampler);
    }

    inline void setSampler2D(int sampler)
    {
        gl::Uniform1i(unif_sampler2d, sampler);
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

