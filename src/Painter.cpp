#include "Painter.hpp"
#include "Window.hpp"
#include "Image.hpp"
#include "ImageUtils.hpp"
#include "LinkedList.hpp"
#include <vector>
#include <cstring>

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


class PainterTexture1DImplImage : public PainterTexture1D, public LinkedList<PainterTexture1DImplImage>::Node{
    PainterImplImage* m_parent_painter   = nullptr;
    
    union Data{
        unsigned char*  uc;
        unsigned short* us;
        unsigned int*   ui;
        float*          f;      
    } m_data;
    
    Type m_data_type;
    
    int m_length           = 0;
    int m_component_count  = 0;

public:
    PainterTexture1DImplImage(PainterImplImage* parent_painter)
    : m_parent_painter(parent_painter)
    {

    }
    
    virtual Type dataType()
    {
#ifdef R64FX_DEBUG
        assert(m_data_type == Type::UnsignedChar);
#endif//R64FX_DEBUG
        return m_data_type;
    }
    
    inline unsigned char* dataUnsignedChar() const
    {
#ifdef R64Fx_DEBUG
        assert(m_data_type == Type::UnsignedShort);
#endif//R64FX_DEBUG
        return m_data.uc;
    }
    
    inline unsigned short* dataUnsignedShort() const
    {
#ifdef R64FX_DEBUG
        assert(m_data_type == Type::UnsignedInt);
#endif//R64FX_DEBUG
        return m_data.us;
    }
    
    inline unsigned int* dataUnsignedInt() const
    {
#ifdef R64FX_DEBUG
        assert(m_data_type == Type::Float);
#endif//R64FX_DEBUG
        return m_data.ui;
    }
    
    inline float* dataFloat() const
    {
        return m_data.f;
    }

    virtual Painter* parentPainter();

    virtual int length()
    {
        return m_length;
    }

    virtual bool isGood()
    {
        return (m_data.f != nullptr && m_length > 0 && m_component_count > 0);
    }

    virtual int componentCount()
    {
        return m_component_count;
    }

    template<typename T> inline void loadData(T* &own_data, T* data, int length, int component_count)
    {
        free();

        m_length = length;
        m_component_count = component_count;

        int nbytes = length * component_count * sizeof(T);
        own_data = new T[nbytes];
        memcpy(own_data, data, nbytes);
    }
    
    virtual void load(unsigned char* data, int length, int component_count)
    {
        loadData<unsigned char>(m_data.uc, data, length, component_count);
        m_data_type = Type::UnsignedChar;
    }

    virtual void load(unsigned short* data, int length, int component_count)
    {
        loadData<unsigned short>(m_data.us, data, length, component_count);
        m_data_type = Type::UnsignedShort;
    }

    virtual void load(unsigned int* data, int length, int component_count)
    {
        loadData<unsigned int>(m_data.ui, data, length, component_count);
        m_data_type = Type::UnsignedInt;
    }

    virtual void load(float* data, int length, int component_count)
    {
        loadData<float>(m_data.f, data, length, component_count);
        m_data_type = Type::Float;
    }

    virtual void free()
    {
        if(m_data.f)
        {
            if(m_data_type == Type::UnsignedChar)
                delete[] m_data.uc;
            else if(m_data_type == Type::UnsignedShort)
                delete[] m_data.us;
            else if(m_data_type == Type::UnsignedInt)
                delete[] m_data.ui;
            else
                delete[] m_data.f;
            m_data.f = nullptr;
            m_length = 0;
            m_component_count = 0;
        }
    }
};


class PainterTexture2DImplImage : public PainterTexture2D, public LinkedList<PainterTexture2DImplImage>::Node{
    PainterImplImage* m_parent_painter;
    Point<int> m_pos;
    Image m_img;
    
public:
    PainterTexture2DImplImage(PainterImplImage* parent_painter)
    : m_parent_painter(parent_painter)
    {

    }

    virtual ~PainterTexture2DImplImage()
    {
        
    }
    
    inline PainterImplImage* painter() const
    {
        return m_parent_painter;
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
    
    virtual bool isGood()
    {
        return m_img.isGood();
    }

    virtual Point<int> position()
    {
        return m_pos;
    }
    
    virtual Size<int> size()
    {
        return {m_img.width(), m_img.height()};
    }
    
    virtual int componentCount()
    {
        return m_img.componentCount();
    }
    
    virtual void loadImage(Image* teximg)
    {
        m_img.load(teximg->width(), teximg->height(), teximg->componentCount(), teximg->data(), true);
    }
    
    virtual void readImage(Image* teximg)
    {
        teximg->load(m_img.width(), m_img.height(), m_img.componentCount(), m_img.data(), true);
    }
    
    virtual void free()
    {
        m_img.free();
    }
};


struct PainterImplImage : public PainterImpl{
    LinkedList<PainterTexture1DImplImage> m_1d_textures;
    LinkedList<PainterTexture2DImplImage> m_2d_textures;

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

    virtual void putImage(Image* img, Point<int> dst_pos)
    {
        RectIntersection<int> intersection(
            current_clip_rect,
            {dst_pos.x() + offsetX(), dst_pos.y() + offsetY(), img->width(), img->height()}
        );

        if(intersection.width() > 0 && intersection.height() > 0)
        {
            implant_alpha(
                window->image(),
                intersection.dstOffset() + current_clip_rect.position(),
                intersection.size(),
                intersection.srcOffset(),
                img
            );
        }
    }

    virtual void putImage(PainterTexture2D* texture, Point<int> dst_pos)
    {
#ifdef R64FX_DEBUG
        assert(texture != nullptr);
        assert(texture->parentPainter() == this);
#endif//R64FX_DEBUG
        
        auto texture_impl = static_cast<PainterTexture2DImplImage*>(texture);
        PainterImplImage::putImage(texture_impl->image(), dst_pos);
    }

    virtual void blendColors(Point<int> pos, unsigned char** colors, Image* mask)
    {
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
                colors, mask
            );
        }
    }

    virtual void blendColors(Point<int> pos, unsigned char** colors, PainterTexture2D* mask)
    {
#ifdef R64FX_DEBUG
        assert(mask != nullptr);
        assert(mask->parentPainter() == this);
#endif//R64FX_DEBUG

        auto mask_texture_impl = static_cast<PainterTexture2DImplImage*>(mask);
        PainterImplImage::blendColors(pos, colors, mask_texture_impl->image());
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
    
    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, PainterTexture1D* waveform, float gain)
    {
#ifdef R64FX_DEBUG
        assert(waveform->length() == rect.width());
        assert(waveform->dataType() == PainterTexture1D::Type::Float);
#endif//R64FX_DEBUG
        
        auto waveform_texture_impl = static_cast<PainterTexture1DImplImage*>(waveform);
        PainterImplImage::drawWaveform(rect, color, waveform_texture_impl->dataFloat(), gain);
    }

    virtual PainterTexture1D* newTexture()
    {
        auto texture_impl = new PainterTexture1DImplImage(this);
        m_1d_textures.append(texture_impl);
        return texture_impl;
    }

    virtual PainterTexture1D* newTexture(unsigned char* data, int length, int component_count)
    {
        auto texture_impl = new PainterTexture1DImplImage(this);
        m_1d_textures.append(texture_impl);
        texture_impl->load(data, length, component_count);
        return texture_impl;
    }

    virtual PainterTexture1D* newTexture(unsigned short* data, int length, int component_count)
    {
        auto texture_impl = new PainterTexture1DImplImage(this);
        m_1d_textures.append(texture_impl);
        texture_impl->load(data, length, component_count);
        return texture_impl;
    }

    virtual PainterTexture1D* newTexture(unsigned int*   data, int length, int component_count)
    {
        auto texture_impl = new PainterTexture1DImplImage(this);
        m_1d_textures.append(texture_impl);
        texture_impl->load(data, length, component_count);
        return texture_impl;
    }

    virtual PainterTexture1D* newTexture(float* data, int length, int component_count)
    {
        auto texture_impl = new PainterTexture1DImplImage(this);
        m_1d_textures.append(texture_impl);
        texture_impl->load(data, length, component_count);
        return texture_impl;
    }

    virtual PainterTexture2D* newTexture(Image* image = nullptr)
    {
        auto texture_impl = new PainterTexture2DImplImage(this);
        m_2d_textures.append(texture_impl);
        if(image)
        {
            texture_impl->loadImage(image);
        }
        return texture_impl;
    }

    virtual void deleteTexture(PainterTexture1D* &texture)
    {
        if(texture->parentPainter() == this)
        {
            auto texture_impl = static_cast<PainterTexture1DImplImage*>(texture);
            m_1d_textures.remove(texture_impl);
            texture_impl->free();
            delete texture_impl;
            texture = nullptr;
        }
    }

    virtual void deleteTexture(PainterTexture2D* &texture)
    {
        if(texture->parentPainter() == this)
        {
            auto texture_impl = static_cast<PainterTexture2DImplImage*>(texture);
            m_2d_textures.remove(texture_impl);
            texture_impl->free();
            delete texture_impl;
            texture = nullptr;
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


Painter* PainterTexture1DImplImage::parentPainter()
{
    return m_parent_painter;
}


Painter* PainterTexture2DImplImage::parentPainter()
{
    return m_parent_painter;
}


#ifdef R64FX_USE_GL
namespace
{
    int PainterImplGL_count = 0;

    bool gl_stuff_is_good = false;

    Shader_Color*       g_Shader_Color      = nullptr;
    Shader_Texture*     g_Shader_Texture    = nullptr;
    Shader_ColorBlend*  g_Shader_ColorBlend = nullptr;

    PainterShader*   g_current_shader  = nullptr;

    const GLuint primitive_restart = 0xFFFF;

    void init_gl_stuff()
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

        g_Shader_ColorBlend = new Shader_ColorBlend;
        if(!g_Shader_ColorBlend->isGood())
            abort();

        gl::Enable(GL_PRIMITIVE_RESTART);
        gl::PrimitiveRestartIndex(primitive_restart);

        gl::Enable(GL_BLEND);
        gl::BlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
    }

    void cleanup_gl_stuff()
    {
        if(g_Shader_Color)
            delete g_Shader_Color;

        if(g_Shader_Texture)
            delete g_Shader_Texture;

        if(g_Shader_ColorBlend)
            delete g_Shader_ColorBlend;
    }
}//namespace


class PainterImplGL;


struct PainterTextureImplGL{
    PainterImplGL*  m_parent_painter   = nullptr;
    GLuint          m_texture          = 0;
    int             m_component_count  = 0;
    
    PainterTextureImplGL(PainterImplGL* parent_painter)
    : m_parent_painter(parent_painter)
    {}
    
    bool formats(int component_count, GLenum &internal_format, GLenum &format)
    {
#ifndef R64FX_DEBUG
        assert(component_count >= 1);
        assert(component_count <= 4);
#endif//R64FX_DEBUG
        
        if(component_count == 1)
        {
            internal_format = GL_R8;
            format = GL_RED;
        }
        else if(m_component_count == 2)
        {
            internal_format = GL_RG8;
            format = GL_RG;
        }
        else if(component_count == 3)
        {
            internal_format = GL_RGB8;
            format = GL_RGB;
        }
        else if(component_count == 4)
        {
            internal_format = GL_RGBA8;
            format = GL_RGBA;
        }
        else
        {
            return false;
        }
        
        return true;
    }
};


class PainterTexture1DImplGL
: public PainterTexture1D
, public LinkedList<PainterTexture1DImplGL>::Node
, public PainterTextureImplGL{
    int m_length = 0;
    int m_length_rcp = 0;
    Type m_data_type;

public:
    PainterTexture1DImplGL(PainterImplGL* parent_painter)
    : PainterTextureImplGL(parent_painter)
    {}
    
    Type dataType()
    {
        return m_data_type;
    }

    virtual Painter* parentPainter();

    virtual bool isGood()
    {
        return false;
    }

    virtual int componentCount()
    {
        return 0;
    }

    virtual void free()
    {
        if(!isGood())
            return;

        gl::DeleteTextures(1, &m_texture);
        m_texture = 0;
    }

    virtual int length()
    {
        return 0;
    }

    template<typename T> void loadData(T* data, int length, int component_count, GLenum type)
    {
#ifdef R64Fx_DEBUG
        assert(data != nullptr);
        assert(length > 0);
        assert(component_count > 0);
#endif//R64FX_DEBUG
        
        free();

        gl::GenTextures(1, &m_texture);
        gl::BindTexture(GL_TEXTURE_1D, m_texture);
        gl::TexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl::TexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        m_component_count = component_count;
        GLenum internal_format, format;
        formats(m_component_count, internal_format, format);
        
        gl::TexStorage1D(
            GL_TEXTURE_2D,
            1,
            internal_format,
            length
        );
        
        gl::PixelStorei(GL_UNPACK_ALIGNMENT, 1);

        gl::TexSubImage1D(
            GL_TEXTURE_1D,
            0,
            0,
            length,
            format,
            type,
            data
        );
        
        m_length = length;
        m_length_rcp = 1.0f / float(length);        
    }
    
    virtual void load(unsigned char* data, int length, int component_count)
    {
        loadData(data, length, component_count, GL_UNSIGNED_BYTE);
        m_data_type = PainterTexture1D::Type::UnsignedChar;
    }
    
    virtual void load(unsigned short*  data, int length, int component_count)
    {
        loadData(data, length, component_count, GL_UNSIGNED_SHORT);
        m_data_type = PainterTexture1D::Type::UnsignedShort;
    }

    virtual void load(unsigned int*    data, int length, int component_count)
    {
        loadData(data, length, component_count, GL_UNSIGNED_INT);
        m_data_type = PainterTexture1D::Type::UnsignedInt;
    }

    virtual void load(float* data, int length, int component_count)
    {
        loadData(data, length, component_count, GL_FLOAT);
        m_data_type = PainterTexture1D::Type::Float;
    }

};//PainterTexture1DImplGL


class PainterTexture2DImplGL
: public PainterTexture2D
, public LinkedList<PainterTexture2DImplGL>::Node
, public PainterTextureImplGL {
    Point<int>      m_posision;
    Size<int>       m_size;
    float           m_wrcp             = 1.0f;
    float           m_hrcp             = 1.0f;
    
public:
    PainterTexture2DImplGL(PainterImplGL* parent_painter)
    : PainterTextureImplGL(parent_painter)
    {

    }

    inline PainterImplGL* painter() const
    {
        return m_parent_painter;
    }
    
    inline int width() const
    {
        return m_size.width();
    }
    
    inline int height() const
    {
        return m_size.height();
    }
    
    inline float wrcp() const
    {
        return m_wrcp;
    }
    
    inline float hrcp() const
    {
        return m_hrcp;
    }
    
    inline void bind()
    {
        gl::BindTexture(GL_TEXTURE_2D, m_texture);
    }
    
    virtual Painter* parentPainter();
    
    virtual bool isGood()
    {
        return m_texture != 0;
    }

    virtual int componentCount()
    {
        return m_component_count;
    }
    
    virtual Point<int> position()
    {
        return m_posision;
    }
    
    virtual Size<int> size()
    {
        return m_size;
    }
    
    virtual void loadImage(Image* teximg)
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
        GLenum internal_format, format;
        formats(m_component_count, internal_format, format);

        gl::TexStorage2D(
            GL_TEXTURE_2D,
            1,
            internal_format,
            teximg->width(), teximg->height()
        );

        gl::PixelStorei(GL_UNPACK_ALIGNMENT, 1);

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

        m_size = {teximg->width(), teximg->height()};
        m_wrcp = 1.0f / float(teximg->width());
        m_hrcp = 1.0f / float(teximg->height());
    }
    
    virtual void readImage(Image* teximg)
    {

    }

    virtual void free()
    {
        if(!isGood())
            return;

        gl::DeleteTextures(1, &m_texture);
        m_texture = 0;
    }

};//PainterTexture2DImplGL


class PainterGLRoutine{
protected:
    GLuint m_vao;
    GLuint m_vbo;

public:
    void init(int nbytes)
    {
        gl::GenVertexArrays(1, &m_vao);
        gl::BindVertexArray(m_vao);
        gl::GenBuffers(1, &m_vbo);
        gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
        gl::BufferData(GL_ARRAY_BUFFER, nbytes, nullptr, GL_STREAM_DRAW);
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


class PainterGLRoutine_ColoredRect : public PainterGLRoutine{
public:
    void init()
    {
        PainterGLRoutine::init(32);
        g_Shader_Color->bindPositionAttr(GL_FLOAT, GL_FALSE, 0, 0);
    }
};


class PainterGLRoutine_TexturedRect : public PainterGLRoutine{
public:
    void init()
    {
        PainterGLRoutine::init(64);
        g_Shader_Texture->bindPositionAttr(GL_FLOAT, GL_FALSE, 0, 0);
        g_Shader_Texture->bindTexCoordAttr(GL_FLOAT, GL_FALSE, 0, 32);
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
};


class PainterGLRoutine_ColorBlend : public PainterGLRoutine_TexturedRect{
public:
    void init()
    {
        PainterGLRoutine::init(64);
        g_Shader_ColorBlend->bindPositionAttr(GL_FLOAT, GL_FALSE, 0, 0);
        g_Shader_ColorBlend->bindTexCoordAttr(GL_FLOAT, GL_FALSE, 0, 32);
    }
};


struct PainterImplGL : public PainterImpl{    
    PainterGLRoutine_ColoredRect   m_colored_rect;
    PainterGLRoutine_TexturedRect  m_textured_rect;
    PainterGLRoutine_ColorBlend    m_color_blend;

    LinkedList<PainterTexture1DImplGL> m_1d_textures;
    LinkedList<PainterTexture2DImplGL> m_2d_textures;

    PainterTexture2DImplGL m_spare_2d_texture;

    float m_window_double_width_rcp = 1.0f;
    float m_window_double_hrcp = 1.0f;
    float m_window_half_width = 0.0f;
    float m_window_half_height = 0.0f;

    PainterImplGL(Window* window)
    : PainterImpl(window)
    , m_spare_2d_texture(this)
    {
        if(PainterImplGL_count == 0)
        {
            init_gl_stuff();
        }
        PainterImplGL_count++;

        m_colored_rect.init();
        m_colored_rect.setRect(0.0f, 0.0f, 1.0f, -1.0f);

        m_textured_rect.init();
        m_textured_rect.setRect(0.0f, 0.0f, 1.0f, -1.0f);

        m_color_blend.init();
        m_color_blend.setRect(0.0f, 0.0f, 1.0f, -1.0f);
    }

    virtual ~PainterImplGL()
    {
        m_colored_rect.cleanup();
        m_textured_rect.cleanup();
        m_color_blend.cleanup();
        
        PainterImplGL_count--;
        if(PainterImplGL_count == 0)
        {
            cleanup_gl_stuff();
        }
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

    void useShader(PainterShader* shader)
    {
        if(g_current_shader != shader)
        {
            g_current_shader = shader;
            g_current_shader->use();
        }
    }

    void setShaderScaleAndShift(PainterShader* shader, const Rect<int> &rect)
    {
        float rw = rect.width()  * m_window_double_width_rcp;
        float rh = rect.height() * m_window_double_hrcp;

        float rx = (rect.x() - m_window_half_width) * m_window_double_width_rcp;
        float ry = (m_window_half_height - rect.y()) * m_window_double_hrcp;

        shader->setScaleAndShift(
            rw, rh, rx, ry
        );
    }

    virtual void fillRect(const Rect<int> &rect, unsigned char* color)
    {
        auto intersection_rect = clip(rect + offset());
        if(intersection_rect.width() > 0 && intersection_rect.height() > 0)
        {
            useShader(g_Shader_Color);
            setShaderScaleAndShift(g_Shader_Color, intersection_rect);

            g_Shader_Color->setColor(
                float(color[0]) * uchar2float_rcp,
                float(color[1]) * uchar2float_rcp,
                float(color[2]) * uchar2float_rcp,
                float(color[3]) * uchar2float_rcp
            );

            m_colored_rect.draw();
        }
    }

    virtual void putImage(Image* img, Point<int> dst_pos)
    {
        m_spare_2d_texture.loadImage(img);
        PainterImplGL::putImage(&m_spare_2d_texture, dst_pos);
    }

    virtual void putImage(PainterTexture2D* texture, Point<int> dst_pos)
    {
#ifdef R64FX_DEBUG
        assert(texture->parentPainter() == this);
#endif//R64FX_DEBUG

        auto tex = static_cast<PainterTexture2DImplGL*>(texture);

        RectIntersection<int> intersection(
            current_clip_rect,
            {dst_pos.x() + offsetX(), dst_pos.y() + offsetY(), tex->width(), tex->height()}
        );

        if(intersection.width() > 0 && intersection.height() > 0)
        {
            useShader(g_Shader_Texture);
            setShaderScaleAndShift(
                g_Shader_Texture, {current_clip_rect.position() + intersection.dstOffset(), intersection.size()}
            );

            gl::ActiveTexture(GL_TEXTURE0);
            tex->bind();
            g_Shader_Texture->setSampler(0);

            m_textured_rect.setTexCoords(
                intersection.srcx() * tex->wrcp(),
                intersection.srcy() * tex->hrcp(),
                (intersection.srcx() + intersection.width())  * tex->wrcp(),
                (intersection.srcy() + intersection.height()) * tex->hrcp()
            );
            m_textured_rect.draw();
        }
    }

    virtual void blendColors(Point<int> dst_pos, unsigned char** colors, Image* mask_image)
    {
        m_spare_2d_texture.loadImage(mask_image);
        PainterImplGL::blendColors(dst_pos, colors, &m_spare_2d_texture);
    }

    virtual void blendColors(Point<int> dst_pos, unsigned char** colors, PainterTexture2D* mask_texture)
    {
#ifdef R64FX_DEBUG
        assert(mask_texture->parentPainter() == this);
#endif//R64FX_DEBUG

        auto mask_texture_impl = static_cast<PainterTexture2DImplGL*>(mask_texture);

        RectIntersection<int> intersection(
            current_clip_rect,
            {dst_pos.x() + offsetX(), dst_pos.y() + offsetY(), mask_texture_impl->width(), mask_texture_impl->height()}
        );

        if(intersection.width() > 0 && intersection.height() > 0)
        {
            useShader(g_Shader_ColorBlend);
            setShaderScaleAndShift(
                g_Shader_ColorBlend, {current_clip_rect.position() + intersection.dstOffset(), intersection.size()}
            );

            gl::ActiveTexture(GL_TEXTURE0);
            mask_texture_impl->bind();
            g_Shader_ColorBlend->setSampler(0);

            m_color_blend.setTexCoords(
                intersection.srcx() * mask_texture_impl->wrcp(),
                intersection.srcy() * mask_texture_impl->hrcp(),
                (intersection.srcx() + intersection.width())  * mask_texture_impl->wrcp(),
                (intersection.srcy() + intersection.height()) * mask_texture_impl->hrcp()
            );

            for(int c=0; c<mask_texture_impl->componentCount(); c++)
            {
                g_Shader_ColorBlend->setColor(
                    float(colors[c][0]) * uchar2float_rcp,
                    float(colors[c][1]) * uchar2float_rcp,
                    float(colors[c][2]) * uchar2float_rcp,
                    float(colors[c][3]) * uchar2float_rcp
                );

                g_Shader_ColorBlend->setTextureComponent(c);

                m_color_blend.draw();
            }
        }
    }

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, float* waveform, float gain)
    {
        
    }

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, PainterTexture1D* waveform, float gain)
    {
        
    }

    virtual PainterTexture1D* newTexture()
    {
        auto texture_impl = new PainterTexture1DImplGL(this);
        m_1d_textures.append(texture_impl);
        return texture_impl;
    }

    virtual PainterTexture1D* newTexture(unsigned char* data, int length, int component_count)
    {
        auto texture_impl = new PainterTexture1DImplGL(this);
        m_1d_textures.append(texture_impl);
        texture_impl->load(data, length, component_count);
        return texture_impl;
    }

    virtual PainterTexture1D* newTexture(unsigned short* data, int length, int component_count)
    {
        auto texture_impl = new PainterTexture1DImplGL(this);
        m_1d_textures.append(texture_impl);
        texture_impl->load(data, length, component_count);
        return texture_impl;
    }

    virtual PainterTexture1D* newTexture(unsigned int*   data, int length, int component_count)
    {
        auto texture_impl = new PainterTexture1DImplGL(this);
        m_1d_textures.append(texture_impl);
        texture_impl->load(data, length, component_count);
        return texture_impl;
    }

    virtual PainterTexture1D* newTexture(float* data, int length, int component_count)
    {
        auto texture_impl = new PainterTexture1DImplGL(this);
        m_1d_textures.append(texture_impl);
        texture_impl->load(data, length, component_count);
        return texture_impl;
    }

    virtual PainterTexture2D* newTexture(Image* image = nullptr)
    {
        auto texture_impl = new PainterTexture2DImplGL(this);
        m_2d_textures.append(texture_impl);
        if(image)
        {
            texture_impl->loadImage(image);
        }
        return texture_impl;
    }

    virtual void deleteTexture(PainterTexture1D* &texture)
    {
        if(texture->parentPainter() == this)
        {
            auto texture_impl = static_cast<PainterTexture1DImplGL*>(texture);
            m_1d_textures.remove(texture_impl);
            texture_impl->free();
            delete texture_impl;
            texture = nullptr;
        }
    }

    virtual void deleteTexture(PainterTexture2D* &texture)
    {
        if(texture->parentPainter() == this)
        {
            auto texture_impl = static_cast<PainterTexture2DImplGL*>(texture);
            m_2d_textures.remove(texture_impl);
            texture_impl->free();
            delete texture_impl;
            texture = nullptr;
        }
    }

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
        m_window_double_hrcp = 2.0f / float(window->height());

        m_window_half_width = window->width() * 0.5f;
        m_window_half_height = window->height() * 0.5f;
    }

};//PainterImplImage


Painter* PainterTexture1DImplGL::parentPainter()
{
    return m_parent_painter;
}


Painter* PainterTexture2DImplGL::parentPainter()
{
    return m_parent_painter;
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
