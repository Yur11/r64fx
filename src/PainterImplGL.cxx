/* To be included in Panter.cpp */

class Shader_rgba;

namespace{
    bool gl_stuff_is_good = false;
    int PainterImplGL_count = 0;

    Shader_rgba* g_Shader_rgba = nullptr;
}


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
    GLuint m_position_vbo;
    GLuint m_color_vbo;

public:
    Shader_rgba() : ShadingProgram(rgba_vert_text, rgba_frag_text)
    {
        if(!isOk())
            return;

        R64FX_GET_ATTRIB_LOCATION(position);
        R64FX_GET_ATTRIB_LOCATION(color);
        R64FX_GET_UNIFORM_LOCATION(sxsytxty);

        gl::GenVertexArrays(1, &m_vao);
        gl::GenBuffers(1, &m_position_vbo);
        gl::GenBuffers(1, &m_color_vbo);

        gl::BindVertexArray(m_vao);
        gl::EnableVertexAttribArray(m_position);
        gl::EnableVertexAttribArray(m_color);

        gl::BindBuffer(GL_ARRAY_BUFFER, m_position_vbo);
        gl::BufferData(GL_ARRAY_BUFFER, 4*2*sizeof(float), nullptr, GL_STREAM_DRAW);
        gl::VertexAttribPointer(m_position, 2, GL_FLOAT, GL_FALSE, 0, 0);

        gl::BindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
        gl::BufferData(GL_ARRAY_BUFFER, 4*4*sizeof(float), nullptr, GL_STREAM_DRAW);
        gl::VertexAttribPointer(m_color, 4, GL_FLOAT, GL_FALSE, 0, 0);

        gl::BindBuffer(GL_ARRAY_BUFFER, 0);
    }


    ~Shader_rgba()
    {
        if(!isOk())
            return;

        gl::DeleteBuffers(1, &m_position_vbo);
        gl::DeleteBuffers(1, &m_color_vbo);
        gl::DeleteVertexArrays(1, &m_vao);
    }

    void setScaleAndShift(float sx, float sy, float tx, float ty)
    {
        gl::Uniform4f(m_sxsytxty, sx, sy, tx, ty);
    }

    void debugDraw()
    {
        float pos[] = {
            100.0f, 100.0f,
            200.0f, 100.0f,
            200.0f, 200.0f,
            100.0f, 200.0f
        };

        float color[] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f
        };

        gl::BindBuffer(GL_ARRAY_BUFFER, m_position_vbo);
        gl::BufferSubData(GL_ARRAY_BUFFER, 0, 4*2*sizeof(float), pos);

        gl::BindBuffer(GL_ARRAY_BUFFER, m_color_vbo);
        gl::BufferSubData(GL_ARRAY_BUFFER, 0, 4*4*sizeof(float), color);

        gl::BindBuffer(GL_ARRAY_BUFFER, 0);

        gl::BindVertexArray(m_vao);

        gl::DrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
};


PainterImplGL::PainterImplGL(Window* window) : PainterImpl(window)
{
    initGLStuffIfNeeded();
    PainterImplGL_count++;
}


PainterImplGL::~PainterImplGL()
{
    cout << "~PainterImplGL\n";

    PainterImplGL_count--;
    if(PainterImplGL_count == 0)
    {
        cleanupGLStuff();
    }
#ifdef R64FX_DEBUG
    else if(PainterImplGL_count <= 0)
    {
        cerr << "Warning PainterImplGL_count is " << PainterImplGL_count << "!\n";
        cerr << "Something is really wrong!\n";
    }
#endif//R64FX_DEBUG
}


void PainterImplGL::debugDraw()
{
    window->makeCurrent();
    gl::Viewport(0, 0, window->width(), window->height());
    gl::ClearColor(1.0, 1.0, 1.0, 0.0);
    gl::Clear(GL_COLOR_BUFFER_BIT);
    g_Shader_rgba->use();
    g_Shader_rgba->setScaleAndShift(
        1.0f/float(window->width()),
       -1.0f/float(window->height()),
       -1.0f,
        1.0f
    );
    g_Shader_rgba->debugDraw();
    window->repaint();
}


void PainterImplGL::repaint()
{
    window->makeCurrent();
    gl::Viewport(0, 0, window->width(), window->height());
    gl::Clear(GL_COLOR_BUFFER_BIT);
    window->repaint();
}


void PainterImplGL::clear()
{
    PainterImpl::clear();
}


void PainterImplGL::initGLStuffIfNeeded()
{
    if(gl_stuff_is_good)
        return;

    int major, minor;
    gl::GetIntegerv(GL_MAJOR_VERSION, &major);
    gl::GetIntegerv(GL_MINOR_VERSION, &minor);
    cout << "gl: " << major << "." << minor << "\n";

    gl::InitIfNeeded();
    gl::ClearColor(1.0, 1.0, 1.0, 0.0);

    g_Shader_rgba = new Shader_rgba;
    if(!g_Shader_rgba->isOk())
        abort();

    gl_stuff_is_good = true;
}


void PainterImplGL::cleanupGLStuff()
{
    cout << "CleanupGLStuff\n";

    if(!gl_stuff_is_good)
        return;

    if(g_Shader_rgba)
        delete g_Shader_rgba;
}


void PaintCommand_FillRect::paintGL(PainterImplGL* impl)
{

}


void PaintCommand_FillRect::configGL(PainterImplGL* impl)
{

}


void PaintCommand_PutImage::paintGL(PainterImplGL* impl)
{

}


void PaintCommand_PutImage::configGL(PainterImplGL* impl)
{

}


void PaintCommand_PutDensePlotHorizontal::paintGL(PainterImplGL* impl)
{

}


void PaintCommand_PutDensePlotHorizontal::configGL(PainterImplGL* impl)
{

}


void PaintCommand_PutDensePlotVertical::paintGL(PainterImplGL* impl)
{

}


void PaintCommand_PutDensePlotVertical::configGL(PainterImplGL* impl)
{

}
