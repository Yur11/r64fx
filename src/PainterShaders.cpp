#ifdef R64FX_USE_GL

#define R64FX_PAINTER_SHADER_IMPL
#include "PainterShaders.hpp"
#undef R64FX_PAINTER_SHADER_IMPL

#include <cstdlib>

#define R64FX_GET_ATTRIB_LOCATION(name) getAttribLocation(attr_##name, #name)
#define R64FX_GET_UNIFORM_LOCATION(name) getUniformLocation(unif_##name, #name)

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

namespace r64fx{

namespace{

const char* text_shader_common_vert =
#include "shader_common.vert.h"
;

const char* text_shader_common_frag =
#include "shader_common.frag.h"
;

VertexShader    g_shader_common_vert;
FragmentShader  g_shader_common_frag;

}//namespace


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


PainterShader_Common::PainterShader_Common()
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


void init_painter_shaders()
{
    g_shader_common_vert = VertexShader(text_shader_common_vert);
    g_shader_common_frag = FragmentShader(text_shader_common_frag);

    g_PainterShader_Common = new PainterShader_Common;
    if(!g_PainterShader_Common->isGood())
        abort();
}


void cleanup_painter_shaders()
{
    if(g_PainterShader_Common)
        delete g_PainterShader_Common;

    g_shader_common_vert.free();
    g_shader_common_frag.free();
}

}//namespace r64fx

#endif//R64FX_USE_GL
