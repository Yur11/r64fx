/* To be included in Painter.cpp */

#include <vector>
#include "PainterVertexArrays.hpp"
#include "Debug.hpp"

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
    PainterVertexArray_V1 m_vert_rect_v1;
    VertexArray_TextureRGBA* m_vert_rect_v2 = nullptr;

    LinkedList<PainterTexture1DImplGL> m_1d_textures;
    LinkedList<PainterTexture2DImplGL> m_2d_textures;

    PainterTexture1DImplGL m_spare_1d_texture;
    PainterTexture2DImplGL m_spare_2d_texture;

    float m_window_double_width_rcp = 1.0f;
    float m_window_minus_double_height_rcp = -1.0f;
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

        m_vert_rect_v1.init();
        m_vert_rect_v1.setRect(0.0f, 0.0f, 1.0f, -1.0f);

        gl::Enable(GL_BLEND);
        gl::BlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

        g_PainterShader_V1->use();
        g_PainterShader_V1->setSampler2D(0);
        g_PainterShader_V1->setSampler1D(1);

        m_vert_rect_v2 = new(std::nothrow) VertexArray_TextureRGBA(4);
        R64FX_DEBUG_ASSERT(m_vert_rect_v2);
    }

    virtual ~PainterImplGL()
    {
        m_vert_rect_v1.cleanup();

        PainterImplGL_count--;
        if(PainterImplGL_count == 0)
        {
            cleanup_gl_stuff();
        }
    }

    virtual void clear(Color color)
    {
        gl::ClearColor(
            float(color[0]) * rcp255,
            float(color[1]) * rcp255,
            float(color[2]) * rcp255,
            float(color[3]) * rcp255
        );
        gl::Clear(GL_COLOR_BUFFER_BIT);
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

    virtual void fillRect(const Rect<int> &rect, Color color)
    {
        auto transformed_rect = clip(rect + offset());
        if(transformed_rect.width() <= 0 || transformed_rect.height() <= 0)
            return;

        g_PainterShader_V1->use();

        g_PainterShader_V1->setMode(PainterShader_V1::ModeColor());

        g_PainterShader_V1->setScaleAndShift(
            m_window_double_width_rcp, m_window_minus_double_height_rcp, -1.0f, +1.0f
        );

        g_PainterShader_V1->setColor(
            float(color[0]) * rcp255,
            float(color[1]) * rcp255,
            float(color[2]) * rcp255,
            float(color[3]) * rcp255
        );

        m_vert_rect_v1.setRect(
            transformed_rect.left(), transformed_rect.top(), transformed_rect.right(), transformed_rect.bottom()
        );

        m_vert_rect_v1.draw();
    }

    virtual void strokeRect(const Rect<int> &rect, Color stroke, Color fill, int stroke_width)
    {
        auto transformed_rect = clip(rect + offset());
        if(transformed_rect.width() <= 0 || transformed_rect.height() <= 0)
            return;

        g_PainterShader_V1->setMode(11);
        g_PainterShader_V1->setScaleAndShift(
            m_window_double_width_rcp, m_window_minus_double_height_rcp, -1.0f, +1.0f
        );

        float colors[8] = {
            float(stroke[0]) * rcp255,
            float(stroke[1]) * rcp255,
            float(stroke[2]) * rcp255,
            float(stroke[3]) * rcp255,
            float(fill[0]) * rcp255,
            float(fill[1]) * rcp255,
            float(fill[2]) * rcp255,
            float(fill[3]) * rcp255
        };

        g_PainterShader_V1->setColors(colors, 0, 2);
        g_PainterShader_V1->setRectSize(transformed_rect.width(), transformed_rect.height());
        g_PainterShader_V1->setStrokeWidth(stroke_width);

        m_vert_rect_v1.setRect(
            transformed_rect.left(), transformed_rect.top(), transformed_rect.right(), transformed_rect.bottom()
        );

        m_vert_rect_v1.setTexCoords(0, 0, transformed_rect.width(), transformed_rect.height());

        m_vert_rect_v1.draw();
    }

    virtual void putImage(Image* image, Point<int> dst_pos, Rect<int> src_rect, FlipFlags flags)
    {
        m_spare_2d_texture.loadImage(image);
        PainterImplGL::putImage(&m_spare_2d_texture, dst_pos, src_rect, flags);
    }

    void putImageOrBlendColors(PainterTexture2D* texture, Point<int> dst_pos, Rect<int> src_rect, FlipFlags flags)
    {
#ifdef R64FX_DEBUG
        assert(texture->parentPainter() == this);
        auto texture_size = texture->size();
        assert(src_rect.x() >= 0);
        assert(src_rect.y() >= 0);
        assert(src_rect.width() <= texture_size.width());
        assert(src_rect.height() <= texture_size.height());
#endif//R64FX_DEBUG

        FlippedIntersection<int> isec(
            current_clip_rect, dst_pos + offset(), src_rect,
            flags & FlipFlags::Vert(), flags & FlipFlags::Hori(), flags & FlipFlags::Diag()
        );
        if(!isec)
            return;

        g_PainterShader_V1->setScaleAndShift(
            m_window_double_width_rcp, m_window_minus_double_height_rcp, -1.0f, +1.0f
        );

        setTexture2D(static_cast<PainterTexture2DImplGL*>(texture));

        m_vert_rect_v1.setTexCoords(
            isec.srcx(),
            isec.srcy(),
            isec.srcx() + isec.srcWidth(),
            isec.srcy() + isec.srcHeight(),
            flags & FlipFlags::Vert(), flags & FlipFlags::Hori(), flags & FlipFlags::Diag()
        );

        m_vert_rect_v1.setRect(
            current_clip_rect.x() + isec.dstx(),
            current_clip_rect.y() + isec.dsty(),
            current_clip_rect.x() + isec.dstx() + isec.dstWidth(),
            current_clip_rect.y() + isec.dsty() + isec.dstHeight()
        );

        m_vert_rect_v1.draw();
    }

    virtual void putImage(PainterTexture2D* texture, Point<int> dst_pos, Rect<int> src_rect, FlipFlags flags)
    {
        g_PainterShader_V1->setMode(PainterShader_V1::ModePutImage(texture->componentCount()));
        putImageOrBlendColors(texture, dst_pos, src_rect, flags);
    }

    virtual void blendColors(Point<int> dst_pos, const Colors &colors, Image* mask_image, FlipFlags flags)
    {
        m_spare_2d_texture.loadImage(mask_image);
        PainterImplGL::blendColors(dst_pos, colors, &m_spare_2d_texture, flags);
    }

    virtual void blendColors(Point<int> dst_pos, const Colors &colors, PainterTexture2D* mask_texture, FlipFlags flags)
    {
        g_PainterShader_V1->setMode(PainterShader_V1::ModeBlendColors(mask_texture->componentCount()));
        for(int c=0; c<mask_texture->componentCount(); c++)
        {
            g_PainterShader_V1->setColor(
                float(colors[c][0]) * rcp255,
                float(colors[c][1]) * rcp255,
                float(colors[c][2]) * rcp255,
                float(colors[c][3]) * rcp255,
                c
            );
        }
        putImageOrBlendColors(mask_texture, dst_pos, {{0, 0}, mask_texture->size()}, flags);
    }

    virtual void drawWaveform(const Rect<int> &rect, Color color, unsigned char* waveform)
    {
        m_spare_1d_texture.load(waveform, rect.width(), 2);
        PainterImplGL::drawWaveform(rect, color, &m_spare_1d_texture);
    }

    virtual void drawWaveform(const Rect<int> &rect, Color color, unsigned short* waveform)
    {
        m_spare_1d_texture.load(waveform, rect.width(), 2);
        PainterImplGL::drawWaveform(rect, color, &m_spare_1d_texture);
    }

    virtual void drawWaveform(const Rect<int> &rect, Color color, unsigned int* waveform)
    {
        m_spare_1d_texture.load(waveform, rect.width(), 2);
        PainterImplGL::drawWaveform(rect, color, &m_spare_1d_texture);
    }

    virtual void drawWaveform(const Rect<int> &rect, Color color, float* waveform)
    {
        m_spare_1d_texture.load(waveform, rect.width(), 2);
        PainterImplGL::drawWaveform(rect, color, &m_spare_1d_texture);
    }

    virtual void drawWaveform(const Rect<int> &rect, Color color, PainterTexture1D* waveform_texture)
    {
// #ifdef R64FX_DEBUG
//         assert(waveform_texture->parentPainter() == this);
//         assert(waveform_texture->componentCount() == 2);
// #endif//R64FX_DEBUG
// 
//         auto waveform_tex = static_cast<PainterTexture1DImplGL*>(waveform_texture);
// 
//         RectIntersection<int> intersection(
//             current_clip_rect,
//             {rect.x() + offsetX(), rect.y() + offsetY(), rect.width(), rect.height()}
//         );
// 
//         if(intersection.width() > 0 && intersection.height() > 0)
//         {
//             setScaleAndShift(
//                 g_PainterShader_V1, {current_clip_rect.position() + intersection.dstOffset(), intersection.size()}
//             );
// 
//             g_PainterShader_V1->setMode(PainterShader_V1::ModeWaveform());
// 
//             g_PainterShader_V1->setColor(
//                 float(color[0]) * rcp255,
//                 float(color[1]) * rcp255,
//                 float(color[2]) * rcp255,
//                 float(color[3]) * rcp255
//             );
// 
//             setTexture1D(waveform_tex);
// 
//             m_vert_rect_v1.setTexCoords(
//                 intersection.srcx(),
//                 intersection.srcy(),
//                 intersection.srcx() + intersection.width(),
//                 intersection.srcy() + intersection.height()
//             );
// 
//             m_vert_rect_v1.draw();
//         }
    }

    virtual void drawPoleZeroPlot(const Rect<int> &rect, PainterTexture1D* texture, int zero_index, int zero_count, int pole_index, int pole_count)
    {
#ifdef R64FX_DEBUG
        assert(texture->parentPainter() == this);
#endif//R64FX_DEBUG

        auto transformed_rect = clip(rect + offset());
        if(transformed_rect.width() <= 0 || transformed_rect.height() <= 0)
            return;

        g_PainterShader_V1->setMode(12);
        g_PainterShader_V1->setScaleAndShift(
            m_window_double_width_rcp, m_window_minus_double_height_rcp, -1.0f, +1.0f
        );

        g_PainterShader_V1->setRectSize(transformed_rect.width(), transformed_rect.height());

        setTexture1D(static_cast<PainterTexture1DImplGL*>(texture));

        g_PainterShader_V1->setZeroIndex(zero_index);
        g_PainterShader_V1->setZeroCount(zero_count);
        g_PainterShader_V1->setPoleIndex(pole_index);
        g_PainterShader_V1->setPoleCount(pole_count);

        m_vert_rect_v1.setRect(
            transformed_rect.left(), transformed_rect.top(), transformed_rect.right(), transformed_rect.bottom()
        );

        m_vert_rect_v1.setTexCoords(0.0f, 0.0f, 1.0f, 1.0f);

        m_vert_rect_v1.draw();
    }

    virtual void tileImage(PainterTexture2D* texture, const Rect<int> &rect) override final
    {
        R64FX_DEBUG_ASSERT(texture);

        auto gltex = static_cast<PainterTexture2DImplGL*>(texture);
        setTexture2D(gltex);

        VertexArray_TextureRGBA::XY pos[4] = {
            (float)rect.left(),   (float)rect.top(),
            (float)rect.right(),  (float)rect.top(),
            (float)rect.left(),   (float)rect.bottom(),
            (float)rect.right(),  (float)rect.bottom()
        };
        m_vert_rect_v2->loadPositions(pos, 0, 4);

        float texw        = gltex->width();
        float texh        = gltex->height();
        float texw_rcp    = 1.0f / texw;
        float texh_rcp    = 1.0f / texh;
        float tex_left    = rect.left()    * texw_rcp;
        float tex_top     = rect.top()     * texh_rcp;
        float tex_right   = rect.right()   * texw_rcp;
        float tex_bottom  = rect.bottom()  * texh_rcp;

        VertexArray_TextureRGBA::XY tex[4] = {
            tex_left,   tex_top,
            tex_right,  tex_top,
            tex_left,   tex_bottom,
            tex_right,  tex_bottom
        };
        m_vert_rect_v2->loadTexCoords(tex, 0, 4);

        m_vert_rect_v2->useShader();
        m_vert_rect_v2->setScaleAndShift(m_window_double_width_rcp, m_window_minus_double_height_rcp, -1.0f, +1.0f);
        m_vert_rect_v2->setSampler2D(0);
        m_vert_rect_v2->draw();
    }

    virtual void tileImage(Image* image, const Rect<int> &rect) override final
    {
        m_spare_2d_texture.loadImage(image);
        PainterImplGL::tileImage(&m_spare_2d_texture, rect);
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

    virtual void deleteTexture(PainterTexture1D* texture)
    {
        if(texture->parentPainter() == this)
        {
            auto texture_impl = static_cast<PainterTexture1DImplGL*>(texture);
            m_1d_textures.remove(texture_impl);
            texture_impl->free();
            delete texture_impl;
        }
    }

    virtual void deleteTexture(PainterTexture2D* texture)
    {
        if(texture->parentPainter() == this)
        {
            auto texture_impl = static_cast<PainterTexture2DImplGL*>(texture);
            m_2d_textures.remove(texture_impl);
            texture_impl->free();
            delete texture_impl;
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

    virtual void setViewport(const Rect<int> &rect)
    {
        gl::Viewport(rect.x(), rect.y(), rect.width(), rect.height());

        setClipRect(rect);
        setOffset(rect.position());

        m_window_double_width_rcp = 2.0f / float(rect.width());
        m_window_minus_double_height_rcp = -2.0f / float(rect.height());

        m_window_half_width = rect.width() * 0.5f;
        m_window_half_height = rect.height() * 0.5f;
    }

    virtual void adjustForWindowSize()
    {
        setViewport({0, 0, window->width(), window->height()});
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
