/* To be included in Painter.cpp */

#include "Shader.hpp"

namespace r64fx{
namespace {


bool gl_stuff_is_good = false;

void init_gl_stuff_needed();

void cleanup_gl_stuff();


#define R64FX_GET_ATTRIB_LOCATION(name) getAttribLocation(m_##name, #name)
#define R64FX_GET_UNIFORM_LOCATION(name) getUniformLocation(m_##name, #name)


const char* rgba_vert_text =
#include "rgba.vert.h"
;

const char* rgba_frag_text =
#include "rgba.frag.h"
;

class Shader_rgba : public ShadingProgram{
    GLint m_position;
    GLint m_color;
    GLint m_sxsytxty;

    GLuint m_vao;

public:
    Shader_rgba() : ShadingProgram(rgba_vert_text, rgba_frag_text)
    {
        if(!isOk())
            return;

        R64FX_GET_ATTRIB_LOCATION(position);
        R64FX_GET_ATTRIB_LOCATION(color);
        R64FX_GET_UNIFORM_LOCATION(sxsytxty);

        gl::GenVertexArrays(1, &m_vao);
    }


    ~Shader_rgba()
    {
        if(!isOk())
            return;

        gl::DeleteVertexArrays(1, &m_vao);
        cout << "DeleteVertexArrays\n";
    }
};

Shader_rgba* g_Shader_rgba = nullptr;


int PainterGL_count = 0;

struct PainterGL : public PainterBase{

    PainterGL(Window* window) : PainterBase(window)
    {
        init_gl_stuff_needed();
        PainterGL_count++;
    }

    virtual ~PainterGL()
    {
        cout << "~PainterGL\n";

        PainterGL_count--;
        if(PainterGL_count == 0)
        {
            cleanup_gl_stuff();
        }
#ifdef R64FX_DEBUG
        else if(PainterGL_count <= 0)
        {
            cerr << "Warning PainterGL_count is " << PainterGL_count << "!\n";
            cerr << "Something is really wrong!\n";
        }
#endif//R64FX_DEBUG
    }

    virtual void fillRect(Rect<int> rect, Color<float> color);

    virtual void putImage(int x, int y, Image* img);

    virtual void putPlot(Rect<int> rect, float* data, int data_size, Orientation orientation);

    virtual void repaint();

    virtual void clear();

};//PainterGL


void PainterGL::fillRect(Rect<int> rect, Color<float> color)
{

}


void PainterGL::putImage(int x, int y, Image* img)
{

}


void PainterGL::putPlot(Rect<int> rect, float* data, int data_size, Orientation orientation)
{

}


void PainterGL::repaint()
{
    window->makeCurrent();
    gl::Viewport(0, 0, window->width(), window->height());
    gl::Clear(GL_COLOR_BUFFER_BIT);
    window->repaint();
}


void PainterGL::clear()
{

}


void init_gl_stuff_needed()
{
    if(gl_stuff_is_good)
        return;

    gl::InitIfNeeded();
    gl::ClearColor(1.0, 0.0, 0.0, 0.0);

    g_Shader_rgba = new Shader_rgba;
    if(!g_Shader_rgba->isOk())
        abort();

    gl_stuff_is_good = true;
}


void cleanup_gl_stuff()
{
    cout << "cleanup_gl_stuff\n";

    if(!gl_stuff_is_good)
        return;

    if(g_Shader_rgba)
        delete g_Shader_rgba;
}

}//namespace
}//namespace r64fx