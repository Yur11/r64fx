#include "Painter.hpp"
#include "Window.hpp"
#include "Image.hpp"
#include "LinkedList.hpp"
#include <vector>

#ifdef R64FX_USE_GL
#include "Shader_rgba.hpp"
#include "Shader_rgba_tex.hpp"
#endif//R64FX_USE_GL

#ifdef R64FX_DEBUG
#include <iostream>
#include <assert.h>
#endif//R64FX_DEBUG

using namespace std;

namespace r64fx{


class PaintContext;

typedef void (*PaintRoutine)(PaintContext* ctx);

/** @brief Collection of values to be passed to paint routines. */
struct PaintContext{
    /** @brief Rectangle given by the user. */
    Rect<int>             orig_rect;

    /** @brief Rectangle relative to the target image. */
    Rect<int>             rect;

    /** @brief Color to use. */
    Color<unsigned char>  color;

    /** @brief Image to put pixels in. */
    Image*                target_image;

    /** @brief Image to read pixels from. */
    Image*                source_image;

    /** @brief Data used to build plots. */
    float*                plot_data;
};


void fill_rect_routine(PaintContext* ctx)
{
    auto img    = ctx->target_image;
    auto rect   = ctx->rect;
    auto color  = ctx->color;

    for(int y=0; y<rect.height(); y++)
    {
        for(int x=0; x<rect.width(); x++)
        {
            img->setPixel(x + rect.x(), y + rect.y(), color.vec);
        }
    }
}


void put_image_routine(PaintContext* ctx)
{
    auto dst   = ctx->target_image;
    auto src   = ctx->source_image;
    auto rect  = ctx->rect;

    for(int y=0; y<rect.height(); y++)
    {
        for(int x=0; x<rect.width(); x++)
        {
            for(int c=0; c<dst->channelCount(); c++)
            {
                dst->pixel(x + rect.x(), y + rect.y())[c] = src->pixel(x, y)[c];
            }
        }
    }
}


struct PainterImpl : public Painter{
    Window*     window    = nullptr;

    /** @brief Reusable PaintContext instance; */
    PaintContext* paint_context = nullptr;

    Rect<int> current_clip_rect;

    PainterImpl(Window* window);

    virtual ~PainterImpl();

    virtual void setClipRect(Rect<int> rect);

    /** @brief Clip a rectangle with the current_clip_rect. */
    Rect<int> clip(Rect<int> rect);

};//PainterImpl


PainterImpl::PainterImpl(Window* window) : window(window)
{
    paint_context = new PaintContext;
}


PainterImpl::~PainterImpl()
{
    delete paint_context;
}


void PainterImpl::setClipRect(Rect<int> rect)
{
    current_clip_rect = rect;
}


Rect<int> PainterImpl::clip(Rect<int> rect)
{
    return intersection(
        current_clip_rect,
        rect
    );
}


struct PainterImplNormal : public PainterImpl{
    PainterImplNormal(Window* window);

    virtual ~PainterImplNormal();

    virtual void fillRect(Color<unsigned char> color, Rect<int> rect);

    virtual void putImage(Image* img, Point<int> pos);

    virtual void repaint(Rect<int>* rects, int numrects);

    virtual void reconfigure();

};//PainterImplNormal


PainterImplNormal::PainterImplNormal(Window* window)
: PainterImpl(window)
{

}


PainterImplNormal::~PainterImplNormal()
{

}


void PainterImplNormal::fillRect(Color<unsigned char> color, Rect<int> rect)
{
    paint_context->rect          = clip(rect + offset());
    paint_context->color         = color;
    fill_rect_routine(paint_context);
}


void PainterImplNormal::putImage(Image* img, Point<int> pos)
{
    paint_context->source_image  = img;
    paint_context->rect          = clip(Rect<int>(pos + offset(), {img->width(), img->height()}));
    put_image_routine(paint_context);
}


void PainterImplNormal::repaint(Rect<int>* rects, int numrects)
{
    window->repaint(rects, numrects);
}


void PainterImplNormal::reconfigure()
{
    paint_context->target_image = window->image();
    setClipRect({0, 0, window->width(), window->height()});
    setOffset({0, 0});
}


#ifdef R64FX_USE_GL
namespace
{
    int PainterImplGL_count = 0;

    bool gl_stuff_is_good = false;

    Shader_rgba*      g_Shader_rgba      = nullptr;
    Shader_rgba_tex*  g_Shader_rgba_tex  = nullptr;

    const GLuint primitive_restart = 0xFFFF;
}


struct PainterImplGL : public PainterImpl{
    GLuint base_texture = 0;
    Size<int> base_texture_size;
    GLuint base_vao;
    GLuint base_vbo;

    PainterImplGL(Window* window);

    virtual ~PainterImplGL();

    virtual void fillRect(Color<unsigned char> color, Rect<int> rect);

    virtual void putImage(Image* img, Point<int> pos);

    virtual void repaint(Rect<int>* rects, int numrects);

    virtual void reconfigure();

    static void initSharedGLStuffIfNeeded();

    static void cleanupSharedGLStuffIfNeeded();

    void initGLStuff();

    void cleanupGLStuff();

    void resizeBaseTextureIfNeeded(int w, int h);

    void deleteBaseTextureIfNeeded();

};//PainterImplNormal


PainterImplGL::PainterImplGL(Window* window)
:PainterImpl(window)
{
    initSharedGLStuffIfNeeded();
    initGLStuff();
    PainterImplGL_count++;
}


PainterImplGL::~PainterImplGL()
{
    cleanupGLStuff();
    PainterImplGL_count--;
    if(PainterImplGL_count == 0)
    {
        cleanupSharedGLStuffIfNeeded();
    }
#ifdef R64FX_DEBUG
    else if(PainterImplGL_count <= 0)
    {
        cerr << "Warning PainterImplGL_count is " << PainterImplGL_count << "!\n";
        cerr << "Something is really wrong!\n";
    }
#endif//R64FX_DEBUG
}


void PainterImplGL::fillRect(Color<unsigned char> color, Rect<int> rect)
{
    Rect<int> r = clip(rect + offset());
    if(r.width() > 0 && r.height() > 0)
    {
        Image img(r.width(), r.height(), 4);

        paint_context->rect = { 0, 0, r.width(), r.height() };
        paint_context->target_image = &img;
        paint_context->color = color;
        fill_rect_routine(paint_context);

        gl::BindTexture(GL_TEXTURE_2D, base_texture);
        gl::TexSubImage2D(
            GL_TEXTURE_2D, 0,
            r.x(), r.y(), r.width(), r.height(),
            GL_RGBA, GL_UNSIGNED_BYTE, img.data()
        );
    }
}


void PainterImplGL::putImage(Image* image, Point<int> pos)
{
    Rect<int> rect(pos + offset(), {image->width(), image->height()});
    Rect<int> r = clip(rect);
    if(r.width() > 0 && r.height() > 0)
    {
        Image img(r.width(), r.height(), 4);

        paint_context->rect = { 0, 0, r.width(), r.height() };
        paint_context->target_image = &img;
        paint_context->source_image = image;
        put_image_routine(paint_context);

        gl::BindTexture(GL_TEXTURE_2D, base_texture);
        gl::TexSubImage2D(
            GL_TEXTURE_2D, 0,
            r.x(), r.y(), r.width(), r.height(),
            GL_RGBA, GL_UNSIGNED_BYTE, img.data()
        );
    }
}


void PainterImplGL::repaint(Rect<int>* rects, int numrects)
{
    gl::BindTexture(GL_TEXTURE_2D, base_texture);
    g_Shader_rgba_tex->use();
    g_Shader_rgba_tex->setScaleAndShift(
        2.0f/float(window->width()),
       -2.0f/float(window->height()),
       -1.0f,
        1.0f
    );
    g_Shader_rgba_tex->setSampler(0);
    gl::BindVertexArray(base_vao);
    gl::DrawArrays(GL_TRIANGLE_FAN, 0, 4);

    window->repaint(rects, numrects);
    gl::Finish();
}


void PainterImplGL::reconfigure()
{
    window->makeCurrent();

    resizeBaseTextureIfNeeded(window->width(), window->height());
    
    gl::Viewport(0, 0, window->width(), window->height());
    gl::Clear(GL_COLOR_BUFFER_BIT);

    float buff[16];

    /* Position. */
    buff[0] = 0.0f;
    buff[1] = 0.0f;
    buff[2] = float(window->width());
    buff[3] = 0.0f;
    buff[4] = float(window->width());
    buff[5] = float(window->height());
    buff[6] = 0.0f;
    buff[7] = float(window->height());

    /* Tex. Coords. */
    buff[8]  = 0.0f;
    buff[9]  = 0.0f;
    buff[10] = float(window->width()) / float(base_texture_size.width());
    buff[11] = 0.0f;
    buff[12] = float(window->width())  / float(base_texture_size.width());
    buff[13] = float(window->height()) / float(base_texture_size.height());
    buff[14] = 0.0f;
    buff[15] = float(window->height()) / float(base_texture_size.height());

    gl::BindBuffer(GL_ARRAY_BUFFER, base_vbo);
    gl::BufferSubData(GL_ARRAY_BUFFER, 0, 64, buff);

    setClipRect({0, 0, window->width(), window->height()});
    setOffset({0, 0});
}


void PainterImplGL::initSharedGLStuffIfNeeded()
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

    g_Shader_rgba_tex = new Shader_rgba_tex;
    if(!g_Shader_rgba_tex->isOk())
        abort();

    gl::Enable(GL_PRIMITIVE_RESTART);
    gl::PrimitiveRestartIndex(primitive_restart);

    gl_stuff_is_good = true;
}


void PainterImplGL::cleanupSharedGLStuffIfNeeded()
{
    if(!gl_stuff_is_good)
        return;

    if(g_Shader_rgba)
        delete g_Shader_rgba;

    if(g_Shader_rgba_tex)
        delete g_Shader_rgba_tex;
}


void PainterImplGL::initGLStuff()
{
    gl::GenVertexArrays(1, &base_vao);
    gl::BindVertexArray(base_vao);
    gl::GenBuffers(1, &base_vbo);
    gl::BindBuffer(GL_ARRAY_BUFFER, base_vbo);
    gl::BufferData(GL_ARRAY_BUFFER, 64, nullptr, GL_STATIC_DRAW);
    gl::EnableVertexAttribArray(g_Shader_rgba_tex->attr_position);
    gl::VertexAttribPointer(
        g_Shader_rgba_tex->attr_position,
        2, GL_FLOAT, GL_FALSE,
        0, 0
    );
    gl::EnableVertexAttribArray(g_Shader_rgba_tex->attr_tex_coord);
    gl::VertexAttribPointer(
        g_Shader_rgba_tex->attr_tex_coord,
        2, GL_FLOAT, GL_FALSE,
        0, 32
    );
}


void PainterImplGL::cleanupGLStuff()
{
    gl::DeleteVertexArrays(1, &base_vao);
    gl::DeleteBuffers(1, &base_vbo);
}


void PainterImplGL::resizeBaseTextureIfNeeded(int w, int h)
{
    bool tex_resize_needed = ( base_texture == 0 || w > base_texture_size.w || h > base_texture_size.h );
    if(!tex_resize_needed)
        return;

    deleteBaseTextureIfNeeded();

    /* Texture width must be divisible by 4 ? */
    while(w & 3)
        w++;

    gl::GenTextures(1, &base_texture);
    gl::BindTexture(GL_TEXTURE_2D, base_texture);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl::TexStorage2D(
        GL_TEXTURE_2D,
        1,
        GL_RGBA8,
        w, h
    );

    base_texture_size = {w, h};
}


void PainterImplGL::deleteBaseTextureIfNeeded()
{
    if(base_texture)
        gl::DeleteTextures(1, &base_texture);
}
#endif//R64FX_USE_GL



Painter* Painter::newInstance(Window* window)
{
    if(window->type() == Window::Type::Normal)
    {
        return new PainterImplNormal(window);
    }
#ifdef R64FX_USE_GL
    else if(window->type() == Window::Type::GL)
    {
        return new PainterImplGL(window);
    }
#endif//R64FX_USE_GL

    return nullptr;
}


void Painter::deleteInstance(Painter* painter)
{
    delete painter;
}


}//namespace r64fx