/* To be included in Painter.cpp */

#include "PainterVertexArrays.hpp"
#include <vector>

namespace r64fx{

class PainterImplGL;

namespace{

long PainterImplGL_count = 0;

constexpr GLuint PRIMITIVE_RESTART_INDEX  = 0xFFFF;
constexpr GLuint MAX_INDEX_COUNT          = 0xFFFE;

void init_gl_stuff()
{
    int major, minor;
    gl::GetIntegerv(GL_MAJOR_VERSION, &major);
    gl::GetIntegerv(GL_MINOR_VERSION, &minor);
    if(major < 3)
    {
        cerr << "OpenGL version " << major << "." << minor << " is to old!\n";
        abort();
    }

    gl::InitIfNeeded();
    init_painter_shaders();

    gl::Enable(GL_PRIMITIVE_RESTART);
    gl::PrimitiveRestartIndex(PRIMITIVE_RESTART_INDEX);
}

void cleanup_gl_stuff()
{
    cleanup_painter_shaders();
}

}//namespace

}//namespace r64fx


#include "PainterTextureImplGL.cxx"


namespace r64fx{

union PainterVertex{
    unsigned long bits = 0;
    struct{
        unsigned short x, y, sx, sy;
    } c;
};

struct PaintLayer : public LinkedList<PaintLayer>::Node{
    std::vector<short> vertex_indices;
};

struct PaintGroup{
    PaintGroup* parent_group = nullptr;
};

PaintGroup* g_spare_paint_groups = nullptr;

PaintGroup* allocPaintGroup()
{
    PaintGroup* pg = nullptr;
    if(g_spare_paint_groups)
    {
        pg = g_spare_paint_groups;
        g_spare_paint_groups = g_spare_paint_groups->parent_group;
    }
    else
    {
        pg = new PaintGroup;
    }
    return pg;
}

void freePaintGroup(PaintGroup* pg)
{
    pg->parent_group = g_spare_paint_groups;
    g_spare_paint_groups = pg;
}


struct PainterImplGL : public PainterImpl{
    PainterVertexArray_CommonRect      m_uber_rect;

    LinkedList<PainterTexture1DImplGL> m_1d_textures;
    LinkedList<PainterTexture2DImplGL> m_2d_textures;

    PainterTexture1DImplGL m_spare_1d_texture;
    PainterTexture2DImplGL m_spare_2d_texture;

    float m_window_double_width_rcp = 1.0f;
    float m_window_double_hrcp = 1.0f;
    float m_window_half_width = 0.0f;
    float m_window_half_height = 0.0f;

    GLuint m_layers_vao;
    GLuint m_layers_vbo;
    LinkedList<PaintLayer> m_layers;
    PaintGroup* m_paint_group = nullptr;

    PainterImplGL(Window* window)
    : PainterImpl(window)
    , m_spare_1d_texture(this)
    , m_spare_2d_texture(this)
    {
        window->makeCurrent();

        if(PainterImplGL_count == 0)
        {
            init_gl_stuff();
        }
        PainterImplGL_count++;

        m_uber_rect.init();
        m_uber_rect.setRect(0.0f, 0.0f, 1.0f, -1.0f);

        gl::Enable(GL_BLEND);
        gl::BlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

        g_PainterShader_Common->use();
        g_PainterShader_Common->setSampler2D(0);
        g_PainterShader_Common->setSampler1D(1);
    }

    virtual ~PainterImplGL()
    {
        m_uber_rect.cleanup();

        PainterImplGL_count--;
        if(PainterImplGL_count == 0)
        {
            cleanup_gl_stuff();
        }
    }

    virtual void clear(unsigned char* color)
    {
        gl::ClearColor(
            float(color[0]) * rcp255,
            float(color[1]) * rcp255,
            float(color[2]) * rcp255,
            float(color[3]) * rcp255
        );
        gl::Clear(GL_COLOR_BUFFER_BIT);
    }

    void setScaleAndShift(PainterShader* shader, const Rect<int> &rect)
    {
        float rw = rect.width()  * m_window_double_width_rcp;
        float rh = rect.height() * m_window_double_hrcp;

        float rx = (rect.x() - m_window_half_width) * m_window_double_width_rcp;
        float ry = (m_window_half_height - rect.y()) * m_window_double_hrcp;

        shader->setScaleAndShift(
            rw, rh, rx, ry
        );
    }

    inline void setTexture1D(PainterTexture1DImplGL* texture)
    {
        gl::ActiveTexture(GL_TEXTURE1);
        texture->bind();
    }

    inline void setTexture2D(PainterTexture2DImplGL* texture)
    {
        gl::ActiveTexture(GL_TEXTURE0);
        texture->bind();
    }

    virtual void fillRect(const Rect<int> &rect, unsigned char* color)
    {
        auto intersection_rect = clip(rect + offset());
        if(intersection_rect.width() > 0 && intersection_rect.height() > 0)
        {
            setScaleAndShift(g_PainterShader_Common, intersection_rect);
            g_PainterShader_Common->setMode(PainterShader_Common::ModeColor());

            g_PainterShader_Common->setColor(
                float(color[0]) * rcp255,
                float(color[1]) * rcp255,
                float(color[2]) * rcp255,
                float(color[3]) * rcp255
            );

            m_uber_rect.draw();
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
            setScaleAndShift(
                g_PainterShader_Common, {current_clip_rect.position() + intersection.dstOffset(), intersection.size()}
            );

            g_PainterShader_Common->setMode(PainterShader_Common::ModePutImage(texture->componentCount()));

            setTexture2D(tex);

            m_uber_rect.setTexCoords(
                intersection.srcx() * tex->wrcp(),
                intersection.srcy() * tex->hrcp(),
                (intersection.srcx() + intersection.width())  * tex->wrcp(),
                (intersection.srcy() + intersection.height()) * tex->hrcp()
            );
            m_uber_rect.draw();
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

        auto mask_tex = static_cast<PainterTexture2DImplGL*>(mask_texture);

        RectIntersection<int> intersection(
            current_clip_rect,
            {dst_pos.x() + offsetX(), dst_pos.y() + offsetY(), mask_tex->width(), mask_tex->height()}
        );

        if(intersection.width() > 0 && intersection.height() > 0)
        {
            setScaleAndShift(
                g_PainterShader_Common, {current_clip_rect.position() + intersection.dstOffset(), intersection.size()}
            );

            setTexture2D(mask_tex);

            m_uber_rect.setTexCoords(
                intersection.srcx() * mask_tex->wrcp(),
                intersection.srcy() * mask_tex->hrcp(),
                (intersection.srcx() + intersection.width())  * mask_tex->wrcp(),
                (intersection.srcy() + intersection.height()) * mask_tex->hrcp()
            );

            for(int c=0; c<mask_tex->componentCount(); c++)
            {
                g_PainterShader_Common->setMode(PainterShader_Common::ModeBlendColors(mask_texture->componentCount()));
                g_PainterShader_Common->setColor(
                    float(colors[c][0]) * rcp255,
                    float(colors[c][1]) * rcp255,
                    float(colors[c][2]) * rcp255,
                    float(colors[c][3]) * rcp255
                );

                m_uber_rect.draw();
            }
        }
    }

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, unsigned char* waveform)
    {
        m_spare_1d_texture.load(waveform, rect.width(), 2);
        PainterImplGL::drawWaveform(rect, color, &m_spare_1d_texture);
    }

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, unsigned short* waveform)
    {
        m_spare_1d_texture.load(waveform, rect.width(), 2);
        PainterImplGL::drawWaveform(rect, color, &m_spare_1d_texture);
    }

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, unsigned int* waveform)
    {
        m_spare_1d_texture.load(waveform, rect.width(), 2);
        PainterImplGL::drawWaveform(rect, color, &m_spare_1d_texture);
    }

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, float* waveform)
    {
        m_spare_1d_texture.load(waveform, rect.width(), 2);
        PainterImplGL::drawWaveform(rect, color, &m_spare_1d_texture);
    }

    virtual void drawWaveform(const Rect<int> &rect, unsigned char* color, PainterTexture1D* waveform_texture)
    {
#ifdef R64FX_DEBUG
        assert(waveform_texture->parentPainter() == this);
        assert(waveform_texture->componentCount() == 2);
#endif//R64FX_DEBUG

        auto waveform_tex = static_cast<PainterTexture1DImplGL*>(waveform_texture);

        RectIntersection<int> intersection(
            current_clip_rect,
            {rect.x() + offsetX(), rect.y() + offsetY(), rect.width(), rect.height()}
        );

        if(intersection.width() > 0 && intersection.height() > 0)
        {
            setScaleAndShift(
                g_PainterShader_Common, {current_clip_rect.position() + intersection.dstOffset(), intersection.size()}
            );

            g_PainterShader_Common->setMode(PainterShader_Common::ModeWaveform());

            g_PainterShader_Common->setColor(
                float(color[0]) * rcp255,
                float(color[1]) * rcp255,
                float(color[2]) * rcp255,
                float(color[3]) * rcp255
            );

            setTexture1D(waveform_tex);

            float wrcp = waveform_tex->lengthRcp();
            float hrcp = 1.0f / float(rect.height());

            m_uber_rect.setTexCoords(
                intersection.srcx() * wrcp,
                intersection.srcy() * hrcp,
                (intersection.srcx() + intersection.width())  * wrcp,
                (intersection.srcy() + intersection.height()) * hrcp
            );

            m_uber_rect.draw();
        }
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

    virtual void deleteTexture(PainterTexture1D** texture)
    {
        if(texture[0]->parentPainter() == this)
        {
            auto texture_impl = static_cast<PainterTexture1DImplGL*>(texture[0]);
            m_1d_textures.remove(texture_impl);
            texture_impl->free();
            delete texture_impl;
            texture[0] = nullptr;
        }
    }

    virtual void deleteTexture(PainterTexture2D** texture)
    {
        if(texture[0]->parentPainter() == this)
        {
            auto texture_impl = static_cast<PainterTexture2DImplGL*>(texture[0]);
            m_2d_textures.remove(texture_impl);
            texture_impl->free();
            delete texture_impl;
            texture[0] = nullptr;
        }
    }

    virtual void beginPaintGroup()
    {
        auto pg = allocPaintGroup();
        pg->parent_group = m_paint_group;
        m_paint_group = pg;
    }

    virtual void endPaintGroup()
    {
        if(m_paint_group)
        {
            auto pg = m_paint_group;
            m_paint_group = pg->parent_group;
            freePaintGroup(pg);
        }
    }

    virtual void resetPaintGroups()
    {
        
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

}//namespace r64fx
