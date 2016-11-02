#include "Painter.hpp"
#include "Window.hpp"
#include "Image.hpp"
#include "ImageUtils.hpp"
#include "LinkedList.hpp"
#include <vector>

#ifdef R64FX_USE_GL
#include "PainterShaders.hpp"
#endif//R64FX_USE_GL

#ifdef R64FX_DEBUG
#include <iostream>
#include <assert.h>
#endif//R64FX_DEBUG

using namespace std;

namespace r64fx{

namespace{
    constexpr float uchar2float_rcp = 1.0f / 255.0f;
}//namespace


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

    virtual void setClipRectAtCurrentOffset(Size<int> size)
    {
        current_clip_rect = {offset(), size};
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


class PainterImplImage;

class PainterTextureImplImage : public PainterTexture, public LinkedList<PainterTextureImplImage>::Node{
    PainterImplImage* m_painter;
    Point<int> m_pos;
    Image m_img;
    
public:
    PainterTextureImplImage(PainterImplImage* painter)
    : m_painter(painter)
    {

    }

    virtual ~PainterTextureImplImage()
    {
        
    }
    
    inline PainterImplImage* painter() const
    {
        return m_painter;
    }
    
    inline int width() const
    {
        return m_img.width();
    }
    
    inline int height() const
    {
        return m_img.height();
    }
    
    inline Image* image()
    {
        return &m_img;
    }
    
    virtual Painter* parentPainter();
    
    virtual bool isGood();
    
    virtual Rect<int> rect();
    
    virtual int componentCount();
    
    virtual void loadImage(Image* teximg);
    
    virtual void readImage(Image* teximg);
    
    virtual void free();
};


struct PainterImplImage : public PainterImpl{
    LinkedList<PainterTextureImplImage> m_textures;
    
    PainterImplImage(Window* window)
    : PainterImpl(window)
    {

    }

    virtual ~PainterImplImage()
    {

    }


    virtual void clear(unsigned char* color)
    {
        fill(window->image(), color, {0, 0, window->width(), window->height()});
    }
    

    virtual void fillRect(const Rect<int> &rect, unsigned char* color)
    {
        auto intersection_rect = clip(rect + offset());
        if(intersection_rect.width() > 0 && intersection_rect.height() > 0)
        {
            fill(window->image(), color, intersection_rect);
        }
    }

    
    virtual PainterTexture* newTexture(Image* image = nullptr)
    {
        auto texture = new PainterTextureImplImage(this);
        m_textures.append(texture);
        if(image)
        {
            texture->loadImage(image);
        }
        return texture;
    }
    

    virtual void deleteTexture(PainterTexture* &texture)
    {
        if(texture->parentPainter() == this)
        {
            auto texture_impl = static_cast<PainterTextureImplImage*>(texture);
            texture_impl->free();
            delete texture_impl;
            texture = nullptr;
        }
    }


    virtual void drawTexture(PainterTexture* texture, Point<int> dst_pos, bool blend_alpha)
    {
        auto texture_impl = static_cast<PainterTextureImplImage*>(texture);
        
        RectIntersection<int> intersection(
            current_clip_rect,
            {dst_pos.x() + offsetX(), dst_pos.y() + offsetY(), texture_impl->width(), texture_impl->height()}
        );
        
        if(intersection.width() > 0 && intersection.height() > 0)
        {
            if(blend_alpha)
            {
                implant_alpha(
                    window->image(),
                    intersection.dstOffset() + current_clip_rect.position(),
                    intersection.size(),
                    intersection.srcOffset(),
                    texture_impl->image()
                );
            }
            else
            {
                implant(
                    window->image(),
                    intersection.dstOffset() + current_clip_rect.position(),
                    intersection.size(),
                    intersection.srcOffset(),
                    texture_impl->image()
                );
            }
        }
    }

    virtual void blendColors(Point<int> pos, unsigned char** colors, PainterTexture* mask_texture)
    {
        auto mask = static_cast<PainterTextureImplImage*>(mask_texture);
        
        RectIntersection<int> intersection(
            current_clip_rect,
            {pos.x() + offsetX(), pos.y() + offsetY(), mask->width(), mask->height()}
        );

        if(intersection.width() > 0 && intersection.height() > 0)
        {
            blend(
                window->image(),
                intersection.dstOffset() + current_clip_rect.position(),
                intersection.size(),
                intersection.srcOffset(),
                colors, mask->image()
            );
        }
    }

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, float* waveform, float gain)
    {
        RectIntersection<int> intersection(
            current_clip_rect, rect + offset()
        );

        if(intersection.width() > 0 && intersection.height() > 0)
        {
            draw_waveform(
                window->image(),
                color,
                waveform + intersection.srcOffset().x() * 2,
                Rect<int>(intersection.dstOffset(), intersection.size()),
                gain
            );
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


Painter* PainterTextureImplImage::parentPainter()
{
    return m_painter;
}


bool PainterTextureImplImage::isGood()
{
    return m_img.isGood();
}


Rect<int> PainterTextureImplImage::rect()
{
    return {m_pos.x(), m_pos.y(), m_img.width(), m_img.height()};
}


int PainterTextureImplImage::componentCount()
{
    return m_img.componentCount();
}


void PainterTextureImplImage::loadImage(Image* teximg)
{
    m_img.load(teximg->width(), teximg->height(), teximg->componentCount(), teximg->data(), true);
}


void PainterTextureImplImage::readImage(Image* teximg)
{
    teximg->load(m_img.width(), m_img.height(), m_img.componentCount(), m_img.data(), true);
}


void PainterTextureImplImage::free()
{
    m_img.free();
}


#ifdef R64FX_USE_GL
namespace
{
    int PainterImplGL_count = 0;

    bool gl_stuff_is_good = false;

    Shader_Color*    g_Shader_Color    = nullptr;
    Shader_Texture*  g_Shader_Texture  = nullptr;

    const GLuint primitive_restart = 0xFFFF;
}


class PainterImplGL;

class PainterTextureImplGL : public PainterTexture, public LinkedList<PainterTextureImplGL>::Node{
    PainterImplGL*  m_painter          = nullptr;
    Rect<int>       m_rect;
    int             m_component_count  = 0;
    GLuint          m_texture          = 0;
    float           m_tex_w            = 1.0f;
    float           m_tex_h            = 1.0f;
    
public:
    PainterTextureImplGL(PainterImplGL* painter) : m_painter(painter)
    {

    }
    
    virtual ~PainterTextureImplGL()
    {

    }
    
    inline PainterImplGL* painter() const
    {
        return m_painter;
    }
    
    inline int width() const
    {
        return m_rect.width();
    }
    
    inline int height() const
    {
        return m_rect.height();
    }
    
    inline void setTexCoordWidth(float width)
    {
        m_tex_w = width;
    }
    
    inline void setTexCoordHeight(float height)
    {
        m_tex_h = height;
    }
    
    inline float texCoordWidth() const
    {
        return m_tex_w;
    }
    
    inline float texCoordHeight() const
    {
        return m_tex_h;
    }
    
    inline void bind()
    {
        gl::BindTexture(GL_TEXTURE_2D, m_texture);
    }
    
    virtual bool isGood();
    
    virtual Painter* parentPainter();
    
    virtual Rect<int> rect();
    
    virtual int componentCount();
    
    virtual void loadImage(Image* teximg);
    
    virtual void readImage(Image* teximg);
    
    virtual void free();
};


//     void initGLStuff()
//     {
//         gl::GenVertexArrays(1, &base_vao);
//         gl::BindVertexArray(base_vao);
//         gl::GenBuffers(1, &base_vbo);
//         gl::BindBuffer(GL_ARRAY_BUFFER, base_vbo);
//         gl::BufferData(GL_ARRAY_BUFFER, 64, nullptr, GL_STATIC_DRAW);
//         gl::EnableVertexAttribArray(g_Shader_Color_tex->attr_position);
//         gl::VertexAttribPointer(
//             g_Shader_Color_tex->attr_position,
//             2, GL_FLOAT, GL_FALSE,
//             0, 0
//         );
//         gl::EnableVertexAttribArray(g_Shader_Color_tex->attr_tex_coord);
//         gl::VertexAttribPointer(
//             g_Shader_Color_tex->attr_tex_coord,
//             2, GL_FLOAT, GL_FALSE,
//             0, 32
//         );
//     }


//     void cleanupGLStuff()
//     {
//         deleteBaseTextureIfNeeded();
//         gl::DeleteVertexArrays(1, &base_vao);
//         gl::DeleteBuffers(1, &base_vbo);
//     }

//     void resizeBaseTextureIfNeeded(int w, int h)
//     {
//         bool tex_resize_needed = ( base_texture == 0 || w > base_texture_size.width() || h > base_texture_size.height() );
//         if(!tex_resize_needed)
//             return;
// 
//         deleteBaseTextureIfNeeded();
// 
//         /* Texture width must be divisible by 4 ? */
//         while(w & 3)
//             w++;
// 
//         gl::GenTextures(1, &base_texture);
//         gl::BindTexture(GL_TEXTURE_2D, base_texture);
//         gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//         gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//         gl::TexStorage2D(
//             GL_TEXTURE_2D,
//             1,
//             GL_RGBA8,
//             w, h
//         );
// 
//         base_texture_size = {w, h};
//         base_texture_image.load(w, h, 4);
//     }
// 
//     void deleteBaseTextureIfNeeded()
//     {
//         if(base_texture)
//             gl::DeleteTextures(1, &base_texture);
//     }


class PainterGL_RGBA{
    GLuint m_vao;
    GLuint m_vbo;

public:
    void init()
    {
        gl::GenVertexArrays(1, &m_vao);
        gl::BindVertexArray(m_vao);
        gl::GenBuffers(1, &m_vbo);
        gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
        gl::BufferData(GL_ARRAY_BUFFER, 32, nullptr, GL_STREAM_DRAW);
        
        g_Shader_Color->bindPositionAttr(GL_FLOAT, GL_FALSE, 0, 0);
    }
    
    void cleanup()
    {
        gl::DeleteVertexArrays(1, &m_vao);
        gl::DeleteBuffers(1, &m_vbo);
    }
    
    void setRect(float left, float top, float right, float bottom)
    { 
        float buff[8];

        buff[0] = left;
        buff[1] = top;

        buff[2] = right;
        buff[3] = top;

        buff[4] = right;
        buff[5] = bottom;

        buff[6] = left;
        buff[7] = bottom;

        gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
        gl::BufferSubData(GL_ARRAY_BUFFER, 0, 32, buff);
    }
    
    void draw()
    {
        gl::BindVertexArray(m_vao);
        gl::DrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
};


class PainterGL_RGBA_Tex{
    GLuint m_vao;
    GLuint m_vbo;

public:
    void init()
    {
        gl::GenVertexArrays(1, &m_vao);
        gl::BindVertexArray(m_vao);
        gl::GenBuffers(1, &m_vbo);
        gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
        gl::BufferData(GL_ARRAY_BUFFER, 64, nullptr, GL_STREAM_DRAW);
        
        g_Shader_Texture->bindPositionAttr(GL_FLOAT, GL_FALSE, 0, 0);
        g_Shader_Texture->bindTexCoordAttr(GL_FLOAT, GL_FALSE, 0, 32);
    }
    
    void cleanup()
    {
        gl::DeleteVertexArrays(1, &m_vao);
        gl::DeleteBuffers(1, &m_vbo);
    }
    
    void setRect(float left, float top, float right, float bottom)
    { 
        float buff[8];

        buff[0] = left;
        buff[1] = top;

        buff[2] = right;
        buff[3] = top;

        buff[4] = right;
        buff[5] = bottom;

        buff[6] = left;
        buff[7] = bottom;

        gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
        gl::BufferSubData(GL_ARRAY_BUFFER, 0, 32, buff);
    }
    
    void setTexCoords(float left, float top, float right, float bottom)
    { 
        float buff[8];

        buff[0] = left;
        buff[1] = top;

        buff[2] = right;
        buff[3] = top;

        buff[4] = right;
        buff[5] = bottom;

        buff[6] = left;
        buff[7] = bottom;

        gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
        gl::BufferSubData(GL_ARRAY_BUFFER, 32, 32, buff);
    }
    
    void draw()
    {
        gl::BindVertexArray(m_vao);
        gl::DrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
};


struct PainterImplGL : public PainterImpl{    
    PainterGL_RGBA     m_rgba_painter;
    PainterGL_RGBA_Tex m_rgba_tex_painter;

    LinkedList<PainterTextureImplGL> m_textures;

    float m_window_double_width_rcp = 1.0f;
    float m_window_double_height_rcp = 1.0f;
    float m_window_half_width = 0.0f;
    float m_window_half_height = 0.0f;


    PainterImplGL(Window* window)
    :PainterImpl(window)
    {
        initSharedGLStuffIfNeeded();
        
        m_rgba_painter.init();
        m_rgba_painter.setRect(0.0f, 0.0f, 1.0f, -1.0f);
        
        m_rgba_tex_painter.init();
        m_rgba_tex_painter.setRect(0.0f, 0.0f, 1.0f, -1.0f);
        
        PainterImplGL_count++;
    }


    virtual ~PainterImplGL()
    {
        m_rgba_painter.cleanup();
        m_rgba_tex_painter.cleanup();
        
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


    virtual void clear(unsigned char* color)
    {
        gl::ClearColor(
            float(color[0]) * uchar2float_rcp,
            float(color[1]) * uchar2float_rcp,
            float(color[2]) * uchar2float_rcp,
            float(color[3]) * uchar2float_rcp
        );
        gl::Clear(GL_COLOR_BUFFER_BIT);
    }


    virtual void fillRect(const Rect<int> &rect, unsigned char* color)
    {
        auto intersection_rect = clip(rect + offset());
        if(intersection_rect.width() > 0 && intersection_rect.height() > 0)
        {
            g_Shader_Color->use();

            float rw = intersection_rect.width()  * m_window_double_width_rcp;
            float rh = intersection_rect.height() * m_window_double_height_rcp;

            float rx = (intersection_rect.x() - m_window_half_width) * m_window_double_width_rcp;
            float ry = (m_window_half_height - intersection_rect.y()) * m_window_double_height_rcp;

            g_Shader_Color->setScaleAndShift(
                rw, rh, rx, ry
            );

            g_Shader_Color->setColor(
                float(color[0]) * uchar2float_rcp,
                float(color[1]) * uchar2float_rcp,
                float(color[2]) * uchar2float_rcp,
                float(color[3]) * uchar2float_rcp
            );

            m_rgba_painter.draw();
        }
    }
    
    
    virtual PainterTexture* newTexture(Image* image = nullptr)
    {
        auto texture = new PainterTextureImplGL(this);
        m_textures.append(texture);
        if(image)
        {
            texture->loadImage(image);
        }
        return texture;
    }

    
    virtual void deleteTexture(PainterTexture* &texture)
    {
        
    }
    
    
    virtual void drawTexture(PainterTexture* texture, Point<int> dst_pos, bool blend_alpha = false)
    {
#ifdef R64FX_DEBUG
        assert(texture->parentPainter() == this);
#endif//R64FX_DEBUG
        
        auto texture_gl = static_cast<PainterTextureImplGL*>(texture);
        
        RectIntersection<int> intersection(
            current_clip_rect,
            {dst_pos.x() + offsetX(), dst_pos.y() + offsetY(), texture_gl->width(), texture_gl->height()}
        );
        
        if(intersection.width() > 0 && intersection.height() > 0)
        {
            g_Shader_Texture->use();
            
            float rw = intersection.width()  * m_window_double_width_rcp;
            float rh = intersection.height() * m_window_double_height_rcp;

            float rx = (intersection.dstx() - m_window_half_width)  * m_window_double_width_rcp;
            float ry = (m_window_half_height - intersection.dsty()) * m_window_double_height_rcp;

            g_Shader_Texture->setScaleAndShift(
                rw, rh, rx, ry
            );
            
            gl::ActiveTexture(GL_TEXTURE0);
            texture_gl->bind();
            g_Shader_Texture->setSampler(0);
            
            m_rgba_tex_painter.setTexCoords(0.0, 0.0, texture_gl->texCoordWidth(), 1.0f);
            m_rgba_tex_painter.draw();
        }
    }
    
    virtual void blendColors(Point<int> pos, unsigned char** colors, PainterTexture* mask_texture)
    {
        
    }
    
//     virtual void putImage(Image* img, Point<int> pos)
//     {
//         RectIntersection<int> intersection(
//             current_clip_rect,
//             {pos.x() + offsetX(), pos.y() + offsetY(), img->width(), img->height()}
//         );
// 
//         if(intersection.width() > 0 && intersection.height() > 0)
//         {
//             implant(
//                 &base_texture_image,
//                 intersection.dstOffset() + current_clip_rect.position(),
//                 intersection.size(),
//                 intersection.srcOffset(),
//                 img
//             );
//             addToBaseTexture(&base_texture_image, {0, 0});
//         }
//     }

//     virtual void blendImage(Image* img, Point<int> pos)
//     {
//         RectIntersection<int> intersection(
//             current_clip_rect,
//             {pos.x() + offsetX(), pos.y() + offsetY(), img->width(), img->height()}
//         );
// 
//         if(intersection.width() > 0 && intersection.height() > 0)
//         {
//             implant_alpha(
//                 &base_texture_image,
//                 intersection.dstOffset() + current_clip_rect.position(),
//                 intersection.size(),
//                 intersection.srcOffset(),
//                 img
//             );
//             addToBaseTexture(&base_texture_image, {0, 0});
//         }
//     }
// 
//     virtual void blendColors(Point<int> pos, unsigned char** colors, Image* mask)
//     {
//         RectIntersection<int> intersection(
//             current_clip_rect,
//             {pos.x() + offsetX(), pos.y() + offsetY(), mask->width(), mask->height()}
//         );
// 
//         if(intersection.width() > 0 && intersection.height() > 0)
//         {
//             blend(
//                 &base_texture_image,
//                 intersection.dstOffset() + current_clip_rect.position(),
//                 intersection.size(),
//                 intersection.srcOffset(),
//                 colors, mask
//             );
//             addToBaseTexture(&base_texture_image, {0, 0});
//         }
//     }


    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, float* waveform, float gain)
    {
//         RectIntersection<int> intersection(
//             current_clip_rect, rect + offset()
//         );
// 
//         if(intersection.width() > 0 && intersection.height() > 0)
//         {
//             draw_waveform(
//                 &base_texture_image,
//                 color,
//                 waveform + intersection.srcOffset().x() * 2,
//                 Rect<int>(intersection.dstOffset(), intersection.size()),
//                 gain
//             );
//         }
//         addToBaseTexture(&base_texture_image, {0, 0});
    }

    
//     void addToBaseTexture(Image* img, Point<int> pos)
//     {
//         gl::BindTexture(GL_TEXTURE_2D, base_texture);
//         gl::TexSubImage2D(
//             GL_TEXTURE_2D, 0,
//             pos.x(), pos.y(), img->width(), img->height(),
//             GL_RGBA, GL_UNSIGNED_BYTE, img->data()
//         );
//     }

    virtual void repaint(Rect<int>* rects, int numrects)
    {        
        window->repaint(rects, numrects);
        gl::Finish();
    }

    virtual void adjustForWindowSize()
    {
        gl::Viewport(0, 0, window->width(), window->height());

        setClipRect({0, 0, window->width(), window->height()});
        setOffset({0, 0});

        m_window_double_width_rcp = 2.0f / float(window->width());
        m_window_double_height_rcp = 2.0f / float(window->height());

        m_window_half_width = window->width() * 0.5f;
        m_window_half_height = window->height() * 0.5f;
    }

    static void initSharedGLStuffIfNeeded()
    {
        if(gl_stuff_is_good)
            return;

        int major, minor;
        gl::GetIntegerv(GL_MAJOR_VERSION, &major);
        gl::GetIntegerv(GL_MINOR_VERSION, &minor);
        if(major < 3)
        {
            cerr << "OpenGL version " << major << "." << minor << " is to old!\n";
            abort();
        }

        gl::InitIfNeeded();

        g_Shader_Color = new Shader_Color;
        if(!g_Shader_Color->isGood())
            abort();

        g_Shader_Texture = new Shader_Texture;
        if(!g_Shader_Texture->isGood())
            abort();

        gl::Enable(GL_PRIMITIVE_RESTART);
        gl::PrimitiveRestartIndex(primitive_restart);
        
        gl::Enable(GL_BLEND);
        gl::BlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

        gl_stuff_is_good = true;
    }

    static void cleanupSharedGLStuffIfNeeded()
    {
        if(!gl_stuff_is_good)
            return;

        if(g_Shader_Color)
            delete g_Shader_Color;

        if(g_Shader_Texture)
            delete g_Shader_Texture;
    }

};//PainterImplImage


bool PainterTextureImplGL::isGood()
{
    return m_texture != 0;
}


Painter* PainterTextureImplGL::parentPainter()
{
    return m_painter;
}


Rect<int> PainterTextureImplGL::rect()
{
    return m_rect;
}


int PainterTextureImplGL::componentCount()
{
    return m_component_count;
}


void PainterTextureImplGL::loadImage(Image* teximg)
{
#ifdef R64FX_DEBUG
    assert(teximg != nullptr);
    assert(teximg->width() > 0);
    assert(teximg->height() > 0);
    assert(teximg->componentCount() > 0);
    assert(teximg->componentCount() <= 4);
#endif//R64FX_DEBUG
    
    free();
    
    gl::GenTextures(1, &m_texture);
    gl::BindTexture(GL_TEXTURE_2D, m_texture);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    m_component_count = teximg->componentCount();
    GLenum internal_format = GL_RGBA8;
    GLenum format = GL_RGBA;
    if(m_component_count == 1)
    {
        internal_format = GL_R8;
        format = GL_RED;
    }
    else if(m_component_count == 2)
    {
        internal_format = GL_RG8;
        format = GL_RG;
    }
    else if(m_component_count == 3)
    {
        internal_format = GL_RGB8;
        format = GL_RGB;
    }
    
    int w = teximg->width();
    int h = teximg->height();
    
    /* Texture width must be divisible by 4 ? */
    while(w & 3)
        w++;
    setTexCoordWidth(float(teximg->width()) / float(w));
    
    gl::TexStorage2D(
        GL_TEXTURE_2D,
        1,
        internal_format,
        w, h
    );
    
    gl::TexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0, 0,
        teximg->width(),
        teximg->height(),
        format,
        GL_UNSIGNED_BYTE,
        teximg->data()
    );
    
    m_rect = {0, 0, teximg->width(), teximg->height()};
}

void PainterTextureImplGL::readImage(Image* teximg)
{
    
}


void PainterTextureImplGL::free()
{
    if(!isGood())
        return;
    
    gl::DeleteTextures(1, &m_texture);
    m_texture = 0;
}
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
