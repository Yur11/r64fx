/* To be included in Painter.cpp */

#include "gl.hpp"

namespace r64fx{
namespace {


class CommonVertexShader : public VertexShader{

};



struct PainterGL : public PainterBase{
    Window* window =  nullptr;

    PainterGL(Window* window)
    {
        this->window = window;
        gl::InitIfNeeded();
        gl::ClearColor(1.0, 0.0, 0.0, 0.0);
    }

    virtual ~PainterGL() {}

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

}//namespace
}//namespace r64fx