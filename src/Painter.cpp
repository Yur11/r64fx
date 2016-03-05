#include "Painter.hpp"
#include "Window.hpp"
#include "Image.hpp"
#include "ImageUtils.hpp"
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

struct PainterImpl : public Painter{
    Window*     window    = nullptr;

    Rect<int> current_clip_rect;

    PainterImpl(Window* window)
    : window(window)
    {

    }

    virtual ~PainterImpl()
    {

    }

    virtual void setClipRect(Rect<int> rect)
    {
        current_clip_rect = rect;
    }

    virtual Rect<int> clipRect()
    {
        return current_clip_rect;
    }

    /** @brief Clip a rectangle with the current_clip_rect. */
    Rect<int> clip(Rect<int> rect)
    {
        return intersection(
            current_clip_rect,
            rect
        );
    }
};//PainterImpl


struct PainterImplImage : public PainterImpl{
    PainterImplImage(Window* window)
    : PainterImpl(window)
    {

    }

    virtual ~PainterImplImage()
    {

    }

    virtual void fillRect(const Rect<int> &rect, unsigned char* color)
    {
        RectIntersection<int> intersection(current_clip_rect, rect + offset());
        if(intersection.width() > 0 && intersection.height() > 0)
        {
            fill(window->image(), color, intersection.dstRect());
        }
    }

    virtual void putImage(Image* img, Point<int> pos)
    {
        RectIntersection<int> intersection(
            current_clip_rect,
            {pos.x() + offsetX(), pos.y() + offsetY(), img->width(), img->height()}
        );
        if(intersection.width() > 0 && intersection.height() > 0)
        {
            implant(window->image(), intersection, img);
        }
    }


    virtual void blendColors(Point<int> pos, unsigned char** colors, Image* mask)
    {
        RectIntersection<int> intersection(
            current_clip_rect,
            {pos.x() + offsetX(), pos.y() + offsetY(), mask->width(), mask->height()}
        );
        if(intersection.width() > 0 && intersection.height() > 0)
        {
            blend(window->image(), intersection, colors, mask);
        }
    }

    virtual void repaint(Rect<int>* rects, int numrects)
    {
        for(int i=0; i<numrects; i++)
        {
            rects[i] = clip(rects[i]);
        }
        window->repaint(rects, numrects);
    }

    virtual void adjustForWindowSize()
    {
        setClipRect({0, 0, window->width(), window->height()});
        setOffset({0, 0});
    }
};//PainterImplImage



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

    PainterImplGL(Window* window)
    :PainterImpl(window)
    {
        initSharedGLStuffIfNeeded();
        initGLStuff();
        PainterImplGL_count++;
    }

    virtual ~PainterImplGL()
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

    virtual void fillRect(const Rect<int> &rect, unsigned char* color)
    {
        RectIntersection<int> intersection(current_clip_rect, rect + offset());
        if(intersection.width() > 0 && intersection.height() > 0)
        {
            Image dst(intersection.width(), intersection.height(), 4);
            if(dst.isGood())
            {
                fill(&dst, color);
                addToBaseTexture(&dst, intersection.dstOffset());
            }
        }
    }

    virtual void putImage(Image* img, Point<int> pos)
    {
        RectIntersection<int> intersection(
            current_clip_rect,
            {pos.x() + offsetX(), pos.y() + offsetY(), img->width(), img->height()}
        );
        if(intersection.width() > 0 && intersection.height() > 0)
        {
            if(intersection.width() == img->width() && intersection.height() == img->height())
            {
                addToBaseTexture(img, pos + offset());
            }
            else
            {
                Image dst(intersection.width(), intersection.height(), 4);
                if(dst.isGood())
                {
                    implant(&dst, {{0, 0}, intersection.size(), intersection.srcOffset()}, img);
                    addToBaseTexture(&dst, intersection.dstOffset());
                }
            }
        }
    }

    virtual void blendColors(Point<int> pos, unsigned char** colors, Image* mask)
    {
        RectIntersection<int> intersection(
            current_clip_rect,
            {pos.x() + offsetX(), pos.y() + offsetY(), mask->width(), mask->height()}
        );
        if(intersection.width() > 0 && intersection.height() > 0)
        {
            Image dst(intersection.width(), intersection.height(), 4);
            if(dst.isGood())
            {
                blend(&dst, {{0, 0}, intersection.size(), intersection.srcOffset()}, colors, mask);
                addToBaseTexture(&dst, intersection.dstOffset());
            }
        }
    }

    void addToBaseTexture(Image* img, Point<int> pos)
    {
        gl::BindTexture(GL_TEXTURE_2D, base_texture);
        gl::TexSubImage2D(
            GL_TEXTURE_2D, 0,
            pos.x(), pos.y(), img->width(), img->height(),
            GL_RGBA, GL_UNSIGNED_BYTE, img->data()
        );
    }

    virtual void repaint(Rect<int>* rects, int numrects)
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

    virtual void adjustForWindowSize()
    {
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

    static void initSharedGLStuffIfNeeded()
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

    static void cleanupSharedGLStuffIfNeeded()
    {
        if(!gl_stuff_is_good)
            return;

        if(g_Shader_rgba)
            delete g_Shader_rgba;

        if(g_Shader_rgba_tex)
            delete g_Shader_rgba_tex;
    }

    void initGLStuff()
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

    void cleanupGLStuff()
    {
        gl::DeleteVertexArrays(1, &base_vao);
        gl::DeleteBuffers(1, &base_vbo);
    }

    void resizeBaseTextureIfNeeded(int w, int h)
    {
        bool tex_resize_needed = ( base_texture == 0 || w > base_texture_size.width() || h > base_texture_size.height() );
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

    void deleteBaseTextureIfNeeded()
    {
        if(base_texture)
            gl::DeleteTextures(1, &base_texture);
    }
};//PainterImplImage
#endif//R64FX_USE_GL


Painter* Painter::newInstance(Window* window)
{
    if(window->type() == Window::Type::Image)
    {
        return new PainterImplImage(window);
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