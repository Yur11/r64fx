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


class PainterShader_V1 : public PainterShader{
    GLint attr_tex_coord;
    GLint unif_mode;
    GLint unif_colors;
    GLint unif_sampler1d;
    GLint unif_sampler2d;
    GLint unif_rect_size;
    GLint unif_stroke_width;
    GLint unif_zero_index;
    GLint unif_zero_count;
    GLint unif_pole_index;
    GLint unif_pole_count;

public:
    PainterShader_V1();

    inline void bindTexCoordAttr(GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer)
    {
        bindAttribute(attr_tex_coord, 2, type, normalized, stride, pointer);
    }

    inline void setMode(int mode)
    {
        gl::Uniform1i(unif_mode, mode);
    }

    inline void setRectSize(float w, float h)
    {
        gl::Uniform2f(unif_rect_size, w, h);
    }

    inline void setStrokeWidth(float w)
    {
        gl::Uniform1f(unif_stroke_width, w);
    }

    inline void setZeroIndex(int i)
    {
        gl::Uniform1i(unif_zero_index, i);
    }

    inline void setZeroCount(int i)
    {
        gl::Uniform1i(unif_zero_count, i);
    }

    inline void setPoleIndex(int i)
    {
        gl::Uniform1i(unif_pole_index, i);
    }

    inline void setPoleCount(int i)
    {
        gl::Uniform1i(unif_pole_count, i);
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

R64FX_DEF_PAINTER_SHADER(PainterShader_V1)


class PainterShader_V2 : public PainterShader{
    PainterShader_V2();
    ~PainterShader_V2(){}
public:
    /* 2D position + texture coords */
    struct Vertex{
        float x = 0.0f, y = 0.0f, s = 0.0f, t = 0.0f;
    };

    /* Simply create new VertexArray instance. Shader will be initialized as needed. */
    class VertexArray{
        GLuint m_vao, m_vbo;
        unsigned int m_vertex_count;

    public:
        VertexArray(unsigned int vertex_count);

        static PainterShader_V2* shader();

        static void useShader();

        static void setScaleAndShift(float sx, float sy, float tx, float ty);

        static void setSampler2D(int sampler);

        void load(PainterShader_V2::Vertex* verts, unsigned int index, unsigned int count);

        void draw();

        ~VertexArray();
    };
};


class VertexArray_TextureRGBA{
    GLuint m_vao, m_vbo;
    unsigned int m_vertex_count;

public:
    struct XY{
        float x = 0.0f, y = 0.0f;
    };

    VertexArray_TextureRGBA(unsigned int vertex_count);

    ~VertexArray_TextureRGBA();

    static void useShader();

    static void setScaleAndShift(float sx, float sy, float tx, float ty);

    static void setSampler2D(int sampler);

    void loadPositions(XY* xy, unsigned int index, unsigned int count);

    void loadTexCoords(XY* xy, unsigned int index, unsigned int count);

    void draw();
};


void init_painter_shaders();

void cleanup_painter_shaders();

}//namespace r64fx

#undef R64FX_PAINTER_SHADER_EXTERN
#undef R64FX_DEF_PAINTER_SHADER

#endif//R64FX_PAINTER_SHADERS_HPP

#endif//R64FX_USE_GL

