#ifdef R64FX_USE_GL

#define R64FX_PAINTER_SHADER_IMPL
#include "PainterShaders.hpp"
#undef R64FX_PAINTER_SHADER_IMPL

#include <cstdlib>

#define R64FX_GET_ATTRIB_LOCATION(name) getAttribLocation(attr_##name, #name)
#define R64FX_GET_UNIFORM_LOCATION(name) getUniformLocation(unif_##name, #name)

#include "Debug.hpp"

namespace r64fx{

namespace{

unsigned char text_shader_common_vert[] =
#   include "shader_common.vert.cxx"
;

unsigned char text_shader_common_frag[] =
#   include "shader_common.frag.cxx"
;

unsigned char text_shader_v2_frag[] =
#   include "shader_v2.frag.cxx"
;

VertexShader    g_shader_common_vert;
FragmentShader  g_shader_common_frag;
FragmentShader  g_shader_v2_frag;

}//namespace


/* === PainterShader_V1 === */

PainterShader::PainterShader(VertexShader vert, FragmentShader frag)
{
    if(load(vert, frag))
    {
        R64FX_GET_UNIFORM_LOCATION(sxsytxty);
        R64FX_GET_ATTRIB_LOCATION(position);
    }
}


PainterShader::~PainterShader()
{

}


PainterShader_V1::PainterShader_V1()
: PainterShader(g_shader_common_vert, g_shader_common_frag)
{
    if(isGood())
    {
        R64FX_GET_ATTRIB_LOCATION(tex_coord);
        R64FX_GET_UNIFORM_LOCATION(mode);
        R64FX_GET_UNIFORM_LOCATION(colors);
        R64FX_GET_UNIFORM_LOCATION(sampler1d);
        R64FX_GET_UNIFORM_LOCATION(sampler2d);
        R64FX_GET_UNIFORM_LOCATION(rect_size);
        R64FX_GET_UNIFORM_LOCATION(stroke_width);
        R64FX_GET_UNIFORM_LOCATION(zero_index);
        R64FX_GET_UNIFORM_LOCATION(zero_count);
        R64FX_GET_UNIFORM_LOCATION(pole_index);
        R64FX_GET_UNIFORM_LOCATION(pole_count);
    }
}



/* === PainterShader_V2 === */
class PainterShader_V2_Impl{
    friend class PainterShader_V2;
    friend class PainterShader_V2::VertexArray;

    PainterShader_V2* shader = nullptr;

    GLint attr_position, attr_tex_coord, unif_sxsytxty, unif_sampler2d;

    long user_count = 0;
} g_PainterShader_V2_Impl;


PainterShader_V2::PainterShader_V2()
: PainterShader(g_shader_common_vert, g_shader_v2_frag)
{
    R64FX_DEBUG_ASSERT(isGood());
    getAttribLocation  (g_PainterShader_V2_Impl.attr_position,  "position");
    getAttribLocation  (g_PainterShader_V2_Impl.attr_tex_coord, "tex_coord");

    getUniformLocation (g_PainterShader_V2_Impl.unif_sxsytxty,  "sxsytxty");
    getUniformLocation (g_PainterShader_V2_Impl.unif_sampler2d, "sampler2d");
}


PainterShader_V2::VertexArray::VertexArray(unsigned int vertex_count)
: m_vertex_count(vertex_count)
{
    if(g_PainterShader_V2_Impl.user_count == 0)
    {
        g_PainterShader_V2_Impl.shader = new(std::nothrow) PainterShader_V2;
    }

    R64FX_DEBUG_ASSERT(g_PainterShader_V2_Impl.shader);
    R64FX_DEBUG_ASSERT(g_PainterShader_V2_Impl.shader->isGood());

    gl::GenVertexArrays(1, &m_vao);
    gl::BindVertexArray(m_vao);
    gl::GenBuffers(1, &m_vbo);
    gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    gl::BufferData(GL_ARRAY_BUFFER, m_vertex_count * sizeof(PainterShader_V2::Vertex), nullptr, GL_STREAM_DRAW);

    gl::EnableVertexAttribArray(g_PainterShader_V2_Impl.attr_position);
    gl::VertexAttribPointer(g_PainterShader_V2_Impl.attr_position, 2, GL_FLOAT, GL_TRUE, 16, 0);

    gl::EnableVertexAttribArray(g_PainterShader_V2_Impl.attr_tex_coord);
    gl::VertexAttribPointer(g_PainterShader_V2_Impl.attr_tex_coord, 2, GL_FLOAT, GL_FALSE, 16, 8);

    g_PainterShader_V2_Impl.user_count++;
}


PainterShader_V2* PainterShader_V2::VertexArray::shader()
{
    R64FX_DEBUG_ASSERT(g_PainterShader_V2_Impl.shader->isGood());
    return g_PainterShader_V2_Impl.shader;
}


void PainterShader_V2::VertexArray::useShader()
{
    g_PainterShader_V2_Impl.shader->use();
}


void PainterShader_V2::VertexArray::setScaleAndShift(float sx, float sy, float tx, float ty)
{
    gl::Uniform4f(g_PainterShader_V2_Impl.unif_sxsytxty, sx, sy, tx, ty);
}


void PainterShader_V2::VertexArray::setSampler2D(int sampler)
{
    gl::Uniform1i(g_PainterShader_V2_Impl.unif_sampler2d, sampler);
}


void PainterShader_V2::VertexArray::load(PainterShader_V2::Vertex* verts, unsigned int index, unsigned int count)
{
    R64FX_DEBUG_ASSERT((index + count) <= m_vertex_count);

    gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    gl::BufferSubData(GL_ARRAY_BUFFER, index * sizeof(PainterShader_V2::Vertex), count * sizeof(PainterShader_V2::Vertex), verts);

//     float v[] = {
//         0.0f, 0.0f,
//         0.0f, 0.0f,
// 
//         100.0f, 0.0f,
//         2.0f, 0.0f,
// 
//         0.0f, 100.0f,
//         0.0f, 2.0f,
// 
//         100.0f, 100.0f,
//         2.0f, 2.0f,
//     };
// 
//     gl::BufferSubData(GL_ARRAY_BUFFER, 0, 64, v);
}


void PainterShader_V2::VertexArray::draw()
{
    gl::BindVertexArray(m_vao);
    gl::DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


PainterShader_V2::VertexArray::~VertexArray()
{
    gl::DeleteVertexArrays(1, &m_vao);
    gl::DeleteBuffers(1, &m_vbo);

    g_PainterShader_V2_Impl.user_count--;

    if(g_PainterShader_V2_Impl.user_count <= 0)
    {
        delete g_PainterShader_V2_Impl.shader;
    }
}



/* ===  VertexArray_TextureRGBA === */
class Shader_TextureRGBA : public ShadingProgram{
    long user_count = 0;

    GLint attr_position, attr_tex_coord, unif_sxsytxty, unif_sampler2d;

    Shader_TextureRGBA()
    {
        load(g_shader_common_vert, g_shader_v2_frag);
        R64FX_DEBUG_ASSERT(isGood());

        getAttribLocation  (attr_position,  "position");
        getAttribLocation  (attr_tex_coord, "tex_coord");

        getUniformLocation (unif_sxsytxty,  "sxsytxty");
        getUniformLocation (unif_sampler2d, "sampler2d");
    }

    friend class VertexArray_TextureRGBA;
} *g_Shader_TextureRGBA = nullptr;


VertexArray_TextureRGBA::VertexArray_TextureRGBA(unsigned int vertex_count)
: m_vertex_count(vertex_count)
{
    if(!g_Shader_TextureRGBA)
        g_Shader_TextureRGBA = new (std::nothrow) Shader_TextureRGBA;

    R64FX_DEBUG_ASSERT(g_Shader_TextureRGBA);
    R64FX_DEBUG_ASSERT(g_Shader_TextureRGBA->isGood());

    gl::GenVertexArrays(1, &m_vao);
    gl::BindVertexArray(m_vao);
    gl::GenBuffers(1, &m_vbo);
    gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    gl::BufferData(GL_ARRAY_BUFFER, m_vertex_count * sizeof(PainterShader_V2::Vertex), nullptr, GL_STREAM_DRAW);

    gl::EnableVertexAttribArray(g_Shader_TextureRGBA->attr_position);
    gl::VertexAttribPointer(g_Shader_TextureRGBA->attr_position, 2, GL_FLOAT, GL_TRUE, 0, 0);

    gl::EnableVertexAttribArray(g_Shader_TextureRGBA->attr_tex_coord);
    gl::VertexAttribPointer(g_Shader_TextureRGBA->attr_tex_coord, 2, GL_FLOAT, GL_FALSE, 0, m_vertex_count * sizeof(XY));

    g_Shader_TextureRGBA->user_count++;
}


VertexArray_TextureRGBA::~VertexArray_TextureRGBA()
{
    gl::DeleteVertexArrays(1, &m_vao);
    gl::DeleteBuffers(1, &m_vbo);

    R64FX_DEBUG_ASSERT(g_Shader_TextureRGBA);

    g_Shader_TextureRGBA->user_count--;

    if(g_Shader_TextureRGBA->user_count <= 0)
    {
        delete g_Shader_TextureRGBA;
        g_Shader_TextureRGBA = nullptr;
    }
}


void VertexArray_TextureRGBA::useShader()
    { g_Shader_TextureRGBA->use(); }

void VertexArray_TextureRGBA::setScaleAndShift(float sx, float sy, float tx, float ty)
    { gl::Uniform4f(g_Shader_TextureRGBA->unif_sxsytxty, sx, sy, tx, ty); }

void VertexArray_TextureRGBA::setSampler2D(int sampler)
    { gl::Uniform1i(g_Shader_TextureRGBA->unif_sampler2d, sampler); }


void VertexArray_TextureRGBA::loadPositions(XY* xy, unsigned int index, unsigned int count)
{
    R64FX_DEBUG_ASSERT((index + count) <= m_vertex_count);
    gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    gl::BufferSubData(GL_ARRAY_BUFFER, index * sizeof(XY), count * sizeof(XY), xy);
}


void VertexArray_TextureRGBA::loadTexCoords(XY* xy, unsigned int index, unsigned int count)
{
    R64FX_DEBUG_ASSERT((index + count) <= m_vertex_count);
    gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    gl::BufferSubData(GL_ARRAY_BUFFER, (index + m_vertex_count) * sizeof(XY), count * sizeof(XY), xy);
}


void VertexArray_TextureRGBA::draw()
{
    gl::BindVertexArray(m_vao);
    gl::DrawArrays(GL_TRIANGLE_STRIP, 0, m_vertex_count);
}



void init_painter_shaders()
{
    g_shader_common_vert  = VertexShader((const char*)text_shader_common_vert);
    g_shader_common_frag  = FragmentShader((const char*)text_shader_common_frag);
    g_shader_v2_frag      = FragmentShader((const char*)text_shader_v2_frag);

    g_PainterShader_V1 = new PainterShader_V1;
    if(!g_PainterShader_V1->isGood())
        abort();
}


void cleanup_painter_shaders()
{
    if(g_PainterShader_V1)
        delete g_PainterShader_V1;

    g_shader_common_vert.free();
    g_shader_common_frag.free();
    g_shader_v2_frag.free();
}

}//namespace r64fx

#endif//R64FX_USE_GL
