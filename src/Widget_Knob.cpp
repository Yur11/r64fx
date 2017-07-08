#include "Widget_Knob.hpp"
#include "Painter.hpp"
#include "TextPainter.hpp"
#include "ImageUtils.hpp"
#include "WidgetFlags.hpp"
#include "FlipFlags.hpp"
#include "RingSectorPainter.hpp"
#include <cmath>
#include <algorithm>

#include <iostream>
using namespace std;

namespace r64fx{

namespace{

float g_2pi_rcp = 0.5f / M_PI;

}//namespace


class KnobAnimation : public LinkedList<KnobAnimation>::Node{
    static LinkedList<KnobAnimation> knob_animations;

    int                m_user_count              = 0;
    int                m_size                    = 0;
    int                m_frame_count             = 0;

    Image*             m_image                   = nullptr;

    PainterTexture2D*  m_texture                 = nullptr;

    struct MarkerFrameCoords{
        short w = 0;
        short h = 0;

        /* Texture coords. */
        short srcx = 0;
        short srcy = 0;

        /* Resulting image coords. */
        short dstx = 0;
        short dsty = 0;
    };
    MarkerFrameCoords*  m_marker_coords     = nullptr;

    KnobAnimation(int size, int frame_count)
    : m_size(size)
    , m_frame_count(frame_count)
    {
        m_image = new Image(m_size * m_frame_count, m_size, 2);
        m_marker_coords = new MarkerFrameCoords[m_frame_count];
        fill(m_image, Color(0, 255));

        genImage(m_image, {0, 0});
        genMarkerFrames(m_image, m_marker_coords, {m_size, 0}, m_size);
    }

    ~KnobAnimation()
    {
        delete m_image;
        delete[] m_marker_coords;
    }

    void genImage(Image* dst, Point<int> dstpos)
    {
#ifdef R64FX_DEBUG
        assert(dst->componentCount() == 2);
#endif//R64FX_DEBUG

        int hs = m_size/2;
        Point<int> center(hs, hs);

        /* Outer ring color. */
        fill({dst, {dstpos.x(), dstpos.y(), hs, m_size}}, Color(31, 0));

        {
            Image c1(hs, m_size, 1);
            fill(&c1, Color(255));
            fill_circle(&c1, 0, 1, Color(0), center, hs - 6.5f);

            Image layer(hs, m_size, 2);
            fill_gradient_vert(&layer, 0, 1, 127, 31, {0, 6, hs, m_size - 6});
            copy(&layer, &c1, ChanShuf(1, 1, 0, 1));

            copy({dst, dstpos}, &layer);
        }

        {
            Image c2(hs, m_size, 1);
            fill(&c2, Color(255));
            fill_circle(&c2, 0, 1, Color(0), center, hs - 8.5f);

            Image layer(hs, m_size, 2);
            fill_gradient_vert(&layer, 0, 1, 223, 31, {0, 8, hs, m_size - 8});
            copy(&layer, &c2, ChanShuf(1, 1, 0, 1));

            copy({dst, dstpos}, &layer);
        }

        {
            Image c3(hs, m_size, 1);
            fill(&c3, Color(255));
            fill_circle(&c3, 0, 1, Color(0), center, hs - 9.5f);

            Image layer(hs, m_size, 2);
            fill_gradient_vert(&layer, 0, 1, 147, 107, {0, 9, hs, m_size - 9});
            copy(&layer, &c3, ChanShuf(1, 1, 0, 1));

            copy({dst, dstpos}, &layer);
        }

        {
            Image alpha_mask(hs, m_size, 1);
            fill(&alpha_mask, Color(0));
            fill_circle(&alpha_mask, 0, 1, Color(255), center, hs);
            invert(&alpha_mask, &alpha_mask);

            copy({dst, dstpos}, &alpha_mask, ChanShuf(1, 1, 0, 1));
        }

        /* Mirror left to right. */
        copy({dst, {dstpos.x() + hs, dstpos.y()}}, {dst, {dstpos.x(), dstpos.y(), hs, m_size}}, ImgCopyFlipHori());
    }

    void genMarkerImage(Image* marker_image)
    {
        marker_image->load(m_size, m_size, 2);
        fill(marker_image, Color(0, 255));
        fill({marker_image, {m_size/2 - 2, 7, 4, m_size/2 - 7}}, Color(0, 127));
        fill({marker_image, {m_size/2 - 1, 8, 2, m_size/2 - 9}}, Color(255, 31));
        for(int i=0; i<4; i++)
        {
            fill({marker_image, {m_size/2 - 1, 7 + i, 2, 1}}, 1, 1, 191 - 24 * i);
        }
    }

    void genMarker(Image* dst, Image* marker_image, Point<int> dstpos, float angle)
    {
#ifdef R64FX_DEBUG
        assert(dst->componentCount() == 2);
#endif//R64FX_DEBUG

        fill(dst, Color(0, 255));
        copyRotated(dst, marker_image, angle);
    }

    void genMarkerFrames(Image* dstimg, MarkerFrameCoords* mc, Point<int> dstpos, int height)
    {
#ifdef R64FX_DEBUG
        assert(m_frame_count == 64);
#endif//R64FX_DEBUG

        Image marker_image;
        genMarkerImage(&marker_image);

        float ang_coeff = (M_PI * 0.5) / m_frame_count;
        int x = dstpos.x();
        int next_x = x;
        int y = dstpos.y();
        for(int i=0; i<m_frame_count; i++)
        {
            Image frame(m_size, m_size, 2);
            genMarker(&frame, &marker_image, {0, 0}, (float(i) * ang_coeff));
            auto rect = fit_content(&frame, Color(0, 255));

            auto &mc = m_marker_coords[i];
            mc.w = rect.width();
            mc.h = rect.height();
            mc.dstx = rect.x();
            mc.dsty = rect.y();

            int xx = x + rect.width();
            if(xx > next_x)
                next_x = xx;

            if((y - dstpos.y() + rect.height()) > height)
            {
                x = next_x;
                y = dstpos.y();
            }

            mc.srcx = x;
            mc.srcy = y;

            copy({dstimg, {mc.srcx, mc.srcy}}, {&frame, rect}, ImgCopyReplace());
            y += rect.height();
        }
    }

    void copyRotated(Image* dst, Image* src, float angle, const ImgCopyFlags flags = ImgCopyReplace())
    {
        int hs = m_size / 2;
        Transformation2D<float> t;
        t.translate(float(hs) - 0.5f, float(hs) - 0.5f);
        t.rotate(-angle);
        t.translate(float(-hs) + 0.5f, float(-hs) + 0.5f);
        copy(dst, t, src, flags);
    }

    bool angle2frame(float in_angle, Point<int> &out_dst_pos, Rect<int> &out_src_rect, FlipFlags &out_flags)
    {
        int n = (256 + 128) - ((in_angle + M_PI) * g_2pi_rcp) * (m_frame_count << 2);
        int m = (n >> 6) & 3;
        n &= 63;

        auto &mc = m_marker_coords[n];

        switch(m)
        {
            case 0:
            {
                out_dst_pos = {mc.dstx, mc.dsty};
                out_src_rect = {mc.srcx, mc.srcy, mc.w, mc.h};
                out_flags = FlipFlags();
                return true;
            }

            case 1:
            {
                out_dst_pos = {mc.dsty, m_size - mc.w - mc.dstx};
                out_src_rect = {mc.srcx, mc.srcy, mc.w, mc.h};
                out_flags = FlipFlags::Vert() | FlipFlags::Diag();
                return true;
            }

            case 2:
            {
                out_dst_pos = {m_size - mc.w - mc.dstx, m_size - mc.h - mc.dsty};
                out_src_rect = {mc.srcx, mc.srcy, mc.w, mc.h};
                out_flags = FlipFlags::Vert() | FlipFlags::Hori();
                return true;
            }

            case 3:
            {
                out_dst_pos = {m_size - mc.h - mc.dsty, mc.dstx};
                out_src_rect = {mc.srcx, mc.srcy, mc.w, mc.h};
                out_flags = FlipFlags::Hori() | FlipFlags::Diag();
                return true;
            }

            default:
                return false;
        }
    }

    int        size()        const { return m_size; }
    int        frameCount()  const { return m_frame_count; }

public:
    inline void loadMarkerFramesTexture(PainterTextureManager* texture_manager)
    {
#ifdef R64FX_DEBUG
        assert(texture_manager != nullptr);
        assert(m_image->isGood());
#endif//R64FX_DEBUG
        m_texture = texture_manager->newTexture(m_image);
    }

    inline bool markerFramesTextureLoaded()
    {
        return m_texture != nullptr;
    }

    float markerAngle(float normalized_value)
    {
        float angle = normalized_value;
        angle *= M_PI * 2.0f;
        angle -= M_PI;
        return angle;
    }

    void paint(Painter* painter, float angle)
    {
        painter->putImage(m_image, {0, 0}, {0, 0, m_size, m_size});

        Image sectorimg(m_size, m_size, 1);
        fill(&sectorimg, Color(0));
        RingSectorPainter rsp(m_size);
        rsp.paint(&sectorimg, -M_PI, angle, m_size/2 - 1.5f, m_size/2 - 6.0f);

        painter->blendColors({0, 0}, Color(127, 191, 255), &sectorimg);

        Point<int> dst_pos;
        Rect<int> src_rect;
        FlipFlags flags = FlipFlags();
        if(angle2frame(angle, dst_pos, src_rect, flags))
        {
            painter->putImage(m_texture, dst_pos, src_rect, FlipFlags(flags));
        }
    }

    void paintArc(Painter* painter, float angle, float length)
    {
#ifdef R64FX_DEBUG
        assert(angle >= -M_PI);
        assert((angle + length) <= +M_PI);
#endif//R64FX_DEBUG
    }

    void debugPaint(Painter* painter, Point<int> position)
    {
        painter->putImage(m_image, position);
    }

    inline void freeMarkerFramesTexture(PainterTextureManager* texture_manager)
    {
#ifdef R64FX_DEBUG
        assert(texture_manager != nullptr);
        assert(m_texture != nullptr);
#endif//R64FX_DEBUG
        texture_manager->deleteTexture(m_texture);
        m_texture = nullptr;
    }

    static KnobAnimation* getAnimation(int size)
    {
        KnobAnimation* anim = nullptr;
        for(auto ka : knob_animations)
        {
            if(ka->size() == size)
            {
                anim = ka;
                break;
            }
        }

        if(!anim)
        {
            anim = new KnobAnimation(size, 64);
            knob_animations.append(anim);
        }
        anim->m_user_count++;
        return anim;
    }

    static void freeAnimation(KnobAnimation* anim)
    {
        anim->m_user_count--;
        if(anim->m_user_count == 0)
        {
            knob_animations.remove(anim);
            delete anim;
        }
    }
};

LinkedList<KnobAnimation> KnobAnimation::knob_animations;


Widget_Knob::Widget_Knob(int size, Widget* parent)
: Widget(parent)
{
    setSize({size, size});
    m_animation = KnobAnimation::getAnimation(size);
    setMinValue(-1.0f);
    setMaxValue(+1.0f);
    setValue(0.0f);
}


Widget_Knob::~Widget_Knob()
{
    if(m_animation)
        KnobAnimation::freeAnimation(m_animation);
}


void Widget_Knob::debugPaint(Painter* painter, Point<int> position, int size)
{
    auto anim = KnobAnimation::getAnimation(size);
    anim->debugPaint(painter, position);
    KnobAnimation::freeAnimation(anim);
}


void Widget_Knob::addedToWindowEvent(WidgetAddedToWindowEvent* event)
{
    if(!m_animation->markerFramesTextureLoaded())
        m_animation->loadMarkerFramesTexture(event->textureManager());
}


void Widget_Knob::removedFromWindowEvent(WidgetRemovedFromWindowEvent* event)
{
    if(m_animation->markerFramesTextureLoaded())
        m_animation->freeMarkerFramesTexture(event->textureManager());
}


void Widget_Knob::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, Color(191, 191, 191, 0));

    m_animation->paint(p, m_animation->markerAngle(normalizedValue()));
}


void Widget_Knob::mousePressEvent(MousePressEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        grabMouseFocus();
    }
}


void Widget_Knob::mouseReleaseEvent(MouseReleaseEvent* event)
{
    if(event->button() == MouseButton::Left() && isMouseFocusOwner())
        releaseMouseFocus();
}


void Widget_Knob::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        setValue(value() + valueStep() * -event->delta().y(), true);
        repaint();
    }
}

}//namespace r64fx
